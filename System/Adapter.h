#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "../Log.h"

#include <dxcapi.h>
#include "../Vector4.h"

#include <memory>

#pragma comment(lib,"dxcompiler.lib")

class Adapter {
public:
	//クライアント領域(ゲーム画面)のサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	void Initialize();
	void Finalize();

	void InitializeDirectXCompiler();

	bool ProcessMessage();

	void DrawTriangle();

	void PreDraw();

	void PostDraw();
private:
	std::shared_ptr<WinApp> winApp_;
	std::shared_ptr<DirectXCommon> directXCommon_;
	// シリアライズしてバイナリにする
	ID3DBlob* signatureBlob_ = nullptr;
	ID3DBlob* errorBlob_ = nullptr;
	// 実際に頂点リソースを作る
	ID3D12Resource* vertexResource_ = nullptr;
	// ビューポート
	D3D12_VIEWPORT viewport_{};
	//シザー矩形
	D3D12_RECT scissorRect_{};

	ID3D12RootSignature* rootSignature_ = nullptr;

	ID3D12PipelineState* graphicsPipelineState_ = nullptr;

	// 頂点バッファビューを作成
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//Shaderをコンパイルする
	IDxcBlob* vertexShaderBlob_ = nullptr;
	IDxcBlob* pixelShaderBlob_ = nullptr;
};

