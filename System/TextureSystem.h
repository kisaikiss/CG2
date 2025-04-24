#pragma once

#include <array>
#include <d3d12.h>
#include <string>
#include <dxgi1_6.h>

#include "externals/DirectXTex/DirectXTex.h"
#include <externals/DirectXTex/d3dx12.h>
#include <Vector2.h>

class TextureSystem {
public:
	~TextureSystem() {
		Finalize();
	}

	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12DescriptorHeap* srvDescriptorHeap);
	void Finalize();

	uint32_t Lord(const std::string& filePath);

	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGpu(uint32_t textureNum ) const { return textures_[textureNum].textureSrvHandleGPU; }

	Vector2 GetTextureSize(uint32_t textureNum) const { return Vector2(textures_[textureNum].width, textures_[textureNum].height); }

private:
	/// <summary>
	/// テクスチャ
	/// </summary>
	class Texture {
	public:
		// テクスチャリソース
		ID3D12Resource* resource;
		ID3D12Resource* intermediateResource;
		// シェーダリソースビューのハンドル(CPU)
		CD3DX12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
		// シェーダリソースビューのハンドル(GPU)
		CD3DX12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
		// 名前
		std::string name;
		//サイズ
		float width{};
		float height{};
		void Finalize();
	};
	ID3D12Device* device_ = nullptr;
	ID3D12GraphicsCommandList* commandList_ = nullptr;
	ID3D12DescriptorHeap* srvDescriptorHeap_ = nullptr;
	uint32_t descriptorSizeSRV_{};
	uint32_t textureNum_ = 0;
	std::array<Texture, 1024> textures_;

};

