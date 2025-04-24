#pragma once
#include <d3d12.h>
#include <VertexData.h>
#include <cstdint>
#include <Matrix4x4.h>
#include <Transform.h>

#include "TextureSystem.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "DirectXCommon.h"

class Camera;

class Sphere {
public:
	static int32_t sphereNum;

	Sphere(DirectXCommon* dxCommon);
	~Sphere();

	void Update();
	void Draw(const Camera& camera);
private:
	static const uint32_t kSubdivision = 16;//分割数
	uint32_t numberOfVertex_{};//頂点数
	uint32_t numberOfIndex_{};//インデックス数
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
	//テクスチャのフラグ
	bool useMonsterBall_ = false;
	//テクスチャ番号
	uint32_t uvChackTextureNum_{};
	uint32_t monsterBallTextureNum_{};

	int32_t myNumber_{};
};

