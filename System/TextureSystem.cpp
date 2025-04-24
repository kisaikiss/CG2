#include "TextureSystem.h"
#include "DirectXUtils.h"

void TextureSystem::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12DescriptorHeap* srvDescriptorHeap) {
	device_ = device;
	commandList_ = commandList;
	srvDescriptorHeap_ = srvDescriptorHeap;
	descriptorSizeSRV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void TextureSystem::Finalize() {
	for (uint32_t i = 0; i < textureNum_; i++) {
		textures_[i].Finalize();
	}
}

uint32_t TextureSystem::Lord(const std::string& filePath) {
	// 読み込み済みテクスチャを検索
	auto it = std::find_if(textures_.begin(), textures_.end(), [&](const auto& texture) {
		return texture.name == filePath;
		});
	if (it != textures_.end()) {
		// 読み込み済みテクスチャの要素番号を取得
		return static_cast<uint32_t>(std::distance(textures_.begin(), it));
	}
	//テクスチャの名前を設定
	textures_[textureNum_].name = filePath;
	//テクスチャを読んで転送する
	DirectX::ScratchImage mipImages = LordTexture(filePath);
	const DirectX::TexMetadata metadata = mipImages.GetMetadata();
	textures_[textureNum_].resource = CreateTextureResource(device_, metadata);
	textures_[textureNum_].intermediateResource = UploadTextureData(textures_[textureNum_].resource, mipImages, device_, commandList_);
	textures_[textureNum_].width = static_cast<float>(metadata.width);
	textures_[textureNum_].height = static_cast<float>(metadata.height);
	//メタデータを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//SRVを作成するディスクリプタヒープの場所を決める
	//0番はIMGUIが使うので+1する
	textures_[textureNum_].textureSrvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap_, descriptorSizeSRV_, textureNum_ + 1);
	textures_[textureNum_].textureSrvHandleGPU = GetGPUDescriptorHandle(srvDescriptorHeap_, descriptorSizeSRV_, textureNum_ + 1);

	//SRVの生成
	device_->CreateShaderResourceView(textures_[textureNum_].resource, &srvDesc, textures_[textureNum_].textureSrvHandleCPU);
	textureNum_++;
	return textureNum_ - 1;
}

void TextureSystem::Texture::Finalize() {
	resource->Release();
	intermediateResource->Release();
}
