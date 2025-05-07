#pragma once
#include "ModelData.h"

#include <d3d12.h>
#include <VertexData.h>
#include <cstdint>
#include <Matrix4x4.h>
#include <Transform.h>
#include <map>

#include "TextureSystem.h"
#include "Material.h"
#include "MaterialData.h"
#include "TransformationMatrix.h"
#include "GraphicsPipelineStateManager.h"
#include <wrl.h>
#include <string>

class Camera;
class Engine;

class Model {
public:
	static int32_t modelNum;

	Model(Engine* engine, const std::string& directoryPath, const std::string& filename);
	~Model();

	/// <summary>
	/// 更新処理(ImGui用)
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera"></param>
	void Draw(const Camera& camera);

	void DrawWithOutline(const Camera& camera);

	/// <summary>
	/// SRTを指定して描画
	/// </summary>
	/// <param name="camera"></param>
	/// <param name="transform"></param>
	void Draw(const Camera& camera, Transforms transform);

	void SetTransform(const Transforms& transform) { transform_ = transform; }

	void SetColor(const Vector4& color) { material_->color = color; }

	/// <summary>
	/// 使わないモデルデータを解放する
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="filename"></param>
	void UnLoadObjFile(const std::string& directoryPath, const std::string& filename);

private:
	//モデルデータ
	//ModelData modelData_{};
	//コマンドリスト(まとまった命令群)
	ID3D12GraphicsCommandList* commandList_ = nullptr;
	//デバイス
	ID3D12Device* device_ = nullptr;
	//PSOマネージャー
	GraphicsPipelineStateManager* graphicsPipelineStateManager_ = nullptr;
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

	//Outline用
	ID3D12Resource* transformationOutlineResource_ = nullptr;
	ID3D12Resource* materialOutlineResource_ = nullptr;
	//wvpデータ
	TransformationMatrix* transformationOutlineData_ = nullptr;
	//マテリアル
	Material* materialOutline_ = nullptr;

	Vector4 color_{};
	//SRT
	Transforms transform_{};
	//uvTransform
	Transforms uvTransform_{};
	//テクスチャのシステム
	TextureSystem* textureSystem_ = nullptr;
	//テクスチャ番号
	uint32_t textureNum_;

	int32_t myNumber_{};
	std::string myModelName_{};
	static inline std::map<std::string, ModelData> modelDatas_;

	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	void DrawOutline(const Camera& camera);

};

