#pragma once
#include <d3d12.h>
#include <VertexData.h>
#include <Material.h>
#include <TransformationMatrix.h>
#include <cstdint>
#include <Matrix4x4.h>
#include <Transform.h>

class Engine;
class Camera;

class Triangle {
public:
	static int32_t triangleNum;

	Triangle(Engine* engine);
	~Triangle();

	void Update();
	void Draw(const Camera& camera);

	/// <summary>
	/// 頂点を指定する
	/// </summary>
	/// <param name="vertex"></param>
	void SetVertex(Vector3 vertex[3]);
private:
	//コマンドリスト(まとまった命令群)
	ID3D12GraphicsCommandList* commandList_ = nullptr;
	//デバイス
	ID3D12Device* device_ = nullptr;
	//リソース
	ID3D12Resource* vertexResource_ = nullptr;
	ID3D12Resource* transformationResource_ = nullptr;
	ID3D12Resource* materialResource_ = nullptr;
	//VBV
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//頂点データ
	VertexData* vertexData_ = nullptr;
	//wvpデータ
	TransformationMatrix* transformationData_ = nullptr;
	//マテリアル
	Material* material_ = nullptr;
	//SRT
	Transforms transform_{};

	int32_t myNumber_{};

};

