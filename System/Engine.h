#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Log.h"

#include <dxcapi.h>
#include "Vector4.h"

#include <memory>

#include "Transform.h"
#include "Matrix4x4.h"
#include "DirectionalLight.h"
#include "TransformationMatrix.h"


#pragma comment(lib,"dxcompiler.lib")

class Engine {
public:
	//クライアント領域(ゲーム画面)のサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	void Initialize();
	void Finalize();

	bool ProcessMessage();

	void UpdateLight();

	void PreDraw();

	void PostDraw();

	void FrameStart();

	/// <summary>
	/// デバイスの取得
	/// </summary>
	/// <returns>デバイス</returns>
	ID3D12Device* GetDevice() const { return directXCommon_->GetDevice(); }

	/// <summary>
	/// 描画コマンドリストの取得
	/// </summary>
	/// <returns>描画コマンドリスト</returns>
	ID3D12GraphicsCommandList* GetCommandList() const { return directXCommon_->GetCommandList(); }

	/// <summary>
	/// テクスチャのシステムを取得
	/// </summary>
	/// <returns></returns>
	TextureSystem* GetTextureSystem() const { return textureSystem_; }

	DirectXCommon* GetDirectXCommon() const { return directXCommon_.get(); }

	WinApp* GetWinApp() const { return winApp_.get(); }

private:
	std::shared_ptr<WinApp> winApp_;
	std::shared_ptr<DirectXCommon> directXCommon_;
	//DXC関連
	IDxcUtils* dxcUtils_ = nullptr;
	IDxcCompiler3* dxcCompiler_ = nullptr;
	IDxcIncludeHandler* includeHandler_ = nullptr;
	// シリアライズしてバイナリにする
	ID3DBlob* signatureBlob_ = nullptr;
	ID3DBlob* errorBlob_ = nullptr;
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

	//テクスチャのシステム
	TextureSystem* textureSystem_ = nullptr;

	//光源
	ID3D12Resource* directionalLightResource_ = nullptr;
	DirectionalLight* directionalLightData_ = nullptr;

	/// <summary>
	/// DXC初期化
	/// </summary>
	void InitializeDirectXCompiler();

	/// <summary>
	/// パイプラインステートオブジェクトの生成
	/// </summary>
	void CreatePSO();

	/// <summary>
	/// 三角形のリソースを生成
	/// </summary>
	void CreateTriangleResource();
};

