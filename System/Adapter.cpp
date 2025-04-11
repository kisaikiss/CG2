#include "Adapter.h"

#include "MatrixCalculations.h"
#include "VertexData.h"


IDxcBlob* CompileShader(
	//CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	//Compilerに使用するProfile
	const wchar_t* profile,
	//初期化で生成したものを3つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler) {
#pragma region 1.hlslファイルを読む
	//hlslファイルの内容ををDXCの機能を利用して読み、コンパイラに渡すための設定を行う
	//これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer{};
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;	//UTF8の文字コードであることを通知
#pragma endregion

#pragma region 2.Compileする
	LPCWSTR arguments[] = {			//コンパイルオプション
		filePath.c_str(),			//コンパイル対象のhlslファイル名
		L"-E", L"main",				//エントリーポイントの指定、基本的にmain以外にはしない
		L"-T", profile,				//ShaderProfileの設定
		L"-Zi", L"-Qembed_debug",	// デバッグ用の情報を埋め込む
		L"-Od",						//最適化を外しておく
		L"-Zpr",					//メモリレイアウトは行優先
	};

	//実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,		//読み込んだファイル
		arguments,					//コンパイルオプション
		_countof(arguments),		//コンパイルオプションの数
		includeHandler,				//includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)	//コンパイル結果
	);

	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));
#pragma endregion

#pragma region 3.警告・エラーがでていないか確認する
	//警告・エラーが出ていたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		//警告・エラー駄目絶対
		assert(false);
	}
#pragma endregion

#pragma region 4.Compile結果を受け取って返す
	//BLOBとはBinaryLargeOBjectの略で、大きなバイナリデータの塊を表す
	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	//もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;
#pragma endregion

}

void Adapter::Initialize() {
	winApp_ = std::make_shared<WinApp>();
	winApp_->Create(L"CG2", L"CG2WindowClass", kClientWidth, kClientHeight);

	directXCommon_ = std::make_shared<DirectXCommon>();
	directXCommon_->Initialize(winApp_.get());

	InitializeDirectXCompiler();

	CreatePSO();

	CreateTriangleResource();
}

void Adapter::Finalize() {
	materialResource_->Release();
	wvpResource_->Release();
	vertexResource_->Release();
	graphicsPipelineState_->Release();
	signatureBlob_->Release();
	if (errorBlob_) {
		errorBlob_->Release();
	}
	rootSignature_->Release();
	pixelShaderBlob_->Release();
	vertexShaderBlob_->Release();
	directXCommon_.reset();

	CloseWindow(winApp_->GetHWND());

	winApp_.reset();
}

void Adapter::InitializeDirectXCompiler() {
	if (!directXCommon_ || !winApp_) {
		return;
	}

	// dxCompilerを初期化
	// DXCはDirectX12とは別のものなので、別で初期化する
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	//現時点ではincludeしないが、includeに対応するための設定を行っておく
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}

void Adapter::CreatePSO() {
	// RootSignature(ShaderとResourceをどのように関連付けるか示したオブジェクト)の作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//DiscriptorRange(利用する範囲を指定して一括で設定を行う機能)
	D3D12_DESCRIPTOR_RANGE descriptorRange[1]{};
	descriptorRange[0].BaseShaderRegister = 0; //0から始まる
	descriptorRange[0].NumDescriptors = 1; //数は一つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //offsetを自動計算


	//RootParameter(データそれぞれのBind情報)の作成
	//複数設定できるので配列。今回は1つだけなので長さ1の配列
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;					//レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange; //テーブルの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//テーブルで利用する数
	descriptionRootSignature.pParameters = rootParameters;				//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);	//配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0; // レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成
	hr = directXCommon_->GetDevice()->CreateRootSignature(0,
		signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));

	//InputLayout(VertexShaderへ渡す頂点データがどのようなものか指定するオブジェクト)
	//floatのVector4でPOSITION0というSemanticsというメンバ変数が定義されている
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//BlendState(PixelShaderからの出力を画面にどのように書き込むか設定する項目)の設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//RasterizerState(Rasterizerに対する設定)の設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderをコンパイルする
	vertexShaderBlob_ = CompileShader(L"Object3d.VS.hlsl", L"vs_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = CompileShader(L"Object3d.PS.hlsl", L"ps_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(pixelShaderBlob_ != nullptr);

	//PSO(Pipeline State Object)を生成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_; // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize() }; // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize() }; // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc; // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; // RasterizerState
	// 書き込みRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	hr = directXCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));

}

void Adapter::CreateTriangleResource() {


	//頂点リソースを作る
	vertexResource_ = CreateBufferResource(directXCommon_->GetDevice(), sizeof(VertexData) * 6);

	//VertexBufferView(ResourceをShaderへの入力頂点として処理するためのView)を作成
	// 頂点バッファビューを作成
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	// 左下
	vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.f,1.f };
	// 上
	vertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.5f,0.f };
	// 右下
	vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.f,1.f };

	// 左下2
	vertexData[3].position = { -0.5f,-0.5f,0.5f,1.0f };
	vertexData[3].texcoord = { 0.f,1.f };
	// 上2
	vertexData[4].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData[4].texcoord = { 0.5f,0.f };
	// 右下2
	vertexData[5].position = { 0.5f,-0.5f,-0.5f,1.0f };
	vertexData[5].texcoord = { 1.f,1.f };

	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	wvpResource_ = CreateBufferResource(directXCommon_->GetDevice(), sizeof(Matrix4x4));
	//データを書き込む
	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列を書き込んでおく
	*wvpData_ = MakeIdentity4x4();

	//透視投影行列
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, static_cast<float>(kClientWidth) / static_cast<float>(kClientHeight), 0.1f, 100.f);

	//マテリアル用のリソースを作る。今回はcolor一つ分のサイズを用意
	materialResource_ = CreateBufferResource(directXCommon_->GetDevice(), sizeof(Vector4));
	//マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//今回は赤を書き込んでみる
	*materialData_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	// ビューポート
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = kClientWidth;
	viewport_.Height = kClientHeight;
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	//シザー矩形
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = kClientWidth;
	scissorRect_.top = 0;
	scissorRect_.bottom = kClientHeight;
}

bool Adapter::ProcessMessage() {
	return winApp_->ProcessMessage();
}

void Adapter::UpdateTriangle() {
	ImGui::Begin("triangle");
	ImGui::DragFloat3("position", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat2("scale", &transform_.scale.x, 0.01f);
	ImGui::ColorEdit3("color", &materialData_->x);
	ImGui::End();
}

void Adapter::DrawTriangle() {
	directXCommon_->GetCommandList()->RSSetViewports(1, &viewport_);	//viewportを設定
	directXCommon_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);//Scirssorを設定
	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	directXCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_);
	directXCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_);	//PSOを設定
	directXCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	directXCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//三角形の移動
	//transform_.rotate.y += 0.03f;
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform_.scale, cameraTransform_.rotate, cameraTransform_.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix_));
	*wvpData_ = worldViewProjectionMatrix;

	//マテリアルCBufferの場所を指定
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//wvp用CBufferの場所を指定
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//SRVのディスクリプタテーブルの先頭を設定。2はルートパラメータ[2]
	directXCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, directXCommon_->GetTextureSrvHandleGpu());
	//描画! (DrawCall)。3頂点で一つのインスタンス。
	directXCommon_->GetCommandList()->DrawInstanced(6, 1, 0, 0);
}

void Adapter::PreDraw() {
	if(directXCommon_)
	directXCommon_->PreDraw();
}

void Adapter::PostDraw() {
	if (directXCommon_)
	directXCommon_->PostDraw();
}

void Adapter::FrameStart() {
	if (directXCommon_) {
		directXCommon_->ImGuiNewFrame();
	}
}
