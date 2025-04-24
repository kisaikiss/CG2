#pragma once
#include <d3d12.h>
#include <VertexData.h>
#include <cstdint>
#include <Matrix4x4.h>
#include <Transform.h>
#include <string>

#include "TextureSystem.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "DirectXCommon.h"

class Sprite {
public:
	Sprite(DirectXCommon* dxCommon, const std::string& fileName);
	~Sprite();

	/// <summary>
	/// スプライトを描画する
	/// </summary>
	/// <param name="position">スプライトの左上座標</param>
	/// <param name="textureNum">テクスチャの値</param>
	void Draw();

	void SetPosition(const Vector2& position);
	void SetSize(float width, float height);
private:
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
	//テクスチャのシステム
	TextureSystem* textureSystem_ = nullptr;

	Vector2 position_{};
	Vector2 size_{};
	uint32_t textureNum_{};
};

