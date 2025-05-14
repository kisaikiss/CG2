#pragma once
#include <d3d12.h>
#include <cstdint>

class Engine;
class Camera;
class MaterialTransformBundle;
class GraphicsPipelineStateManager;
class TextureSystem;
struct VertexData;

class TriangularPyramid {
public:
	TriangularPyramid(Engine* engine);
	~TriangularPyramid() = default;
	void Draw(Camera* camera);

	static void Initialize(Engine* engine);
	static void Finalize();

	void SetMaterialTransform(MaterialTransformBundle* materialTransform) {
		materialTransformResource = materialTransform;
	}

	void SetTextureNum(uint32_t textureNum) { textureNum_ = textureNum; }
private:
	//コマンドリスト(まとまった命令群)
	ID3D12GraphicsCommandList* commandList_ = nullptr;
	//PSOマネージャー
	GraphicsPipelineStateManager* graphicsPipelineStateManager_ = nullptr;
	//リソース
	static inline ID3D12Resource* vertexResource_;
	static inline ID3D12Resource* indexResource_;
	MaterialTransformBundle* materialTransformResource = nullptr;
	//VBV
	static inline D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//IBV
	static inline D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	//頂点データ
	static inline VertexData* vertexData_;
	//indexデータ
	static inline uint32_t* indexData_;
	//テクスチャのシステム
	TextureSystem* textureSystem_ = nullptr;
	uint32_t textureNum_;
};

