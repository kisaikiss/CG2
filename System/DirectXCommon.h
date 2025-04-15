#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <format>
#include <dxgidebug.h>

#include "externals/DirectXTex/DirectXTex.h"
#include <externals/DirectXTex/d3dx12.h>

#include "WinApp.h"
#include "../ConvertString.h"

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
	ID3D12Device* GetDevice() const { return device_; }

	/// <summary>
	/// 描画コマンドリストの取得
	/// </summary>
	/// <returns>描画コマンドリスト</returns>
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_; }


	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGpu() const { return textureSrvHandleGpu_; }

private:
	WinApp* winApp_ = nullptr;
	HRESULT hr_;
	ID3D12Device* device_ = nullptr;
	ID3D12Debug1* debugController_ = nullptr;
	//DXGIファクトリーの生成
	IDXGIFactory7* dxgiFactory_ = nullptr;
	// 使用するアダプタ用の変数。最初にnullptrを入れておく
	IDXGIAdapter4* useAdapter_ = nullptr;
	//コマンドキュー(コマンドリストをGPUに投げて実行させる物)を生成する
	ID3D12CommandQueue* commandQueue_ = nullptr;
	//コマンドリスト(まとまった命令群)を生成する
	ID3D12GraphicsCommandList* commandList_ = nullptr;
	//コマンドアロケータ(命令保存用のメモリ管理機構)を生成する
	ID3D12CommandAllocator* commandAllocator_ = nullptr;
	//ディスクリプタヒープの生成
	ID3D12DescriptorHeap* rtvDescriptorHeap_ = nullptr;
	ID3D12DescriptorHeap* srvDescriptorHeap_ = nullptr;
	ID3D12DescriptorHeap* dsvDescriptorHeap_ = nullptr;
	// スワップチェーンを生成する
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	IDXGISwapChain4* swapChain_ = nullptr;
	//SwapChainからResourceを引っ張ってくる
	ID3D12Resource* swapChainResources_[2] = { nullptr };
	// RTVを2個作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2]{};
	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
	// 初期値0でFenceを作る
	ID3D12Fence* fence_ = nullptr;
	uint64_t fenceValue_ = 0;
	// FenceのSignalを待つためのイベントを作成する
	HANDLE fenceEvent_ = nullptr;
	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier_{};

	//テクスチャリソース
	ID3D12Resource* textureResource_ = nullptr;
	//mip
	DirectX::ScratchImage mipImages_{};

	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGpu_{};

	ID3D12Resource* intermediateResource_ = nullptr;

	ID3D12Resource* depthStencilResource_ = nullptr;

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
	/// シェーダリソースビューを生成
	/// </summary>
	void CreateShaderResourceView();

	/// <summary>
	/// DSVの生成
	/// </summary>
	void CreateDepthStencilView();

	/// <summary>
	/// ImGui初期化
	/// </summary>
	void InitializeImGui();

	/// <summary>
	/// フェンス生成
	/// </summary>
	void CreateFence();

};