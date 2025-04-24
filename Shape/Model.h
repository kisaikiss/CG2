#pragma once
#include "ModelData.h"

#include <d3d12.h>
#include <VertexData.h>
#include <cstdint>
#include <Matrix4x4.h>
#include <Transform.h>

#include "TextureSystem.h"
#include "Material.h"
#include "MaterialData.h"
#include "TransformationMatrix.h"
#include "DirectXCommon.h"
#include <wrl.h>
#include <string>

class Camera;

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

class Model {
public:
	static int32_t modelNum;

	Model(DirectXCommon* dxCommon, const std::string& directoryPath, const std::string& filename);
	~Model();

	void Update();
	void Draw(const Camera& camera);
private:
	//モデルデータ
	ModelData modelData_{};
	//コマンドリスト(まとまった命令群)
	ID3D12GraphicsCommandList* commandList_ = nullptr;
	//デバイス
	ID3D12Device* device_ = nullptr;
	//リソース
	ID3D12Resource* vertexResource_ = nullptr;
	ID3D12Resource* indexResource_ = nullptr;
	ID3D12Resource* transformationResource_ = nullptr;
	ID3D12Resource* materialResource_ = nullptr;
	//VBV
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//IBV
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	//頂点データ
	VertexData* vertexData_ = nullptr;
	//wvpデータ
	TransformationMatrix* transformationData_ = nullptr;
	//indexデータ
	uint32_t* indexData_ = nullptr;
	//マテリアル
	Material* material_ = nullptr;
	//SRT
	Transforms transform_{};
	//uvTransform
	Transforms uvTransform_{};
	//テクスチャのシステム
	TextureSystem* textureSystem_ = nullptr;
	//テクスチャ番号
	uint32_t textureNum_;

	int32_t myNumber_{};
};

