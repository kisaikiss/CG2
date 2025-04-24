#pragma once
#define DIRECTINPUT_VERSION		0x0800
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <format>
#include <dxgidebug.h>
#include <memory>
#include "TextureSystem.h"
#include <wrl.h>

#include "externals/DirectXTex/DirectXTex.h"
#include <externals/DirectXTex/d3dx12.h>

#include "WinApp.h"
#include "ConvertString.h"

/// <summary>
/// DirectX
/// </summary>
class DirectXCommon {
public:

	~DirectXCommon();

	void Initialize(WinApp* winApp, int32_t backBufferWidth = WinApp::kWindowWidth, int32_t backBufferHeight = WinApp::kWindowHeight);

	void PreDraw();

	void PostDraw();

	void ImGuiNewFrame();

	/// <summary>
	/// デバイスの取得
	/// </summary>
	/// <returns>デバイス</returns>
	ID3D12Device* GetDevice() const { return device_.Get(); }

	/// <summary>
	/// 描画コマンドリストの取得
	/// </summary>
	/// <returns>描画コマンドリスト</returns>
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }

	/// <summary>
	/// テクスチャのシステムを取得
	/// </summary>
	/// <returns></returns>
	TextureSystem* GetTextureSystem() const { return textureSystem_.get(); }

private:
	WinApp* winApp_ = nullptr;
	HRESULT hr_;
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController_ = nullptr;
	//DXGIファクトリーの生成
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	// 使用するアダプタ用の変数。最初にnullptrを入れておく
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;
	//コマンドキュー(コマンドリストをGPUに投げて実行させる物)を生成する
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;
	//コマンドリスト(まとまった命令群)を生成する
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	//コマンドアロケータ(命令保存用のメモリ管理機構)を生成する
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	//ディスクリプタヒープの生成
	ID3D12DescriptorHeap* rtvDescriptorHeap_ = nullptr;
	ID3D12DescriptorHeap* srvDescriptorHeap_ = nullptr;
	ID3D12DescriptorHeap* dsvDescriptorHeap_ = nullptr;
	// スワップチェーンを生成する
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	//IDXGISwapChain4* swapChain_ = nullptr;
	//SwapChainからResourceを引っ張ってくる
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2] = { nullptr };
	// RTVを2個作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2]{};
	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
	// 初期値0でFenceを作る
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;
	uint64_t fenceValue_ = 0;
	// FenceのSignalを待つためのイベントを作成する
	HANDLE fenceEvent_ = nullptr;
	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier_{};

	ID3D12Resource* depthStencilResource_ = nullptr;
	
	//テクスチャのシステム]
	std::shared_ptr<TextureSystem> textureSystem_;

	//ディスクリプタのサイズ
	uint32_t descriptorSizeSRV_{};
	uint32_t descriptorSizeRTV_{};
	uint32_t descriptorSizeDSV_{};

	//バックバッファの大きさ
	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;

	/// <summary>
	/// DXGIデバイス初期化
	/// </summary>
	void InitializeDXGIDevice();

	/// <summary>
	/// コマンド関連初期化
	/// </summary>
	void InitializeCommand();

	/// <summary>
	/// スワップチェーンを生成
	/// </summary>
	void CreateSwapChain();

	/// <summary>
	/// レンダーターゲットを生成
	/// </summary>
	void CreateFinalRenderTargets();

	/// <summary>
	/// テクスチャのシステムを初期化
	/// </summary>
	void InitializeTextureSystem();

	/// <summary>
	/// DSVの生成
	/// </summary>
	void CreateDepthStencilView();

	void InitializeInput();

	/// <summary>
	/// ImGui初期化
	/// </summary>
	void InitializeImGui();

	/// <summary>
	/// フェンス生成
	/// </summary>
	void CreateFence();

};