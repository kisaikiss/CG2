#pragma once
#include <d3d12.h>
#include <VertexData.h>
#include <cstdint>
#include <Matrix4x4.h>
#include <Transform.h>

class Camera;

class Sphere {
public:
	static int32_t sphereNum;

	Sphere() = default;
	~Sphere();

	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	void Update();
	void Draw(const Camera& camera);
private:
	static const uint32_t kSubdivision = 16;//分割数
	uint32_t numberOfVertex_{};//頂点数
	//コマンドリスト(まとまった命令群)
	ID3D12GraphicsCommandList* commandList_ = nullptr;
	//デバイス
	ID3D12Device* device_ = nullptr;
	//リソース
	ID3D12Resource* vertexResource_ = nullptr;
	ID3D12Resource* wvpResource_ = nullptr;
	ID3D12Resource* materialResource_ = nullptr;
	//VBV
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//頂点データ
	VertexData* vertexData_ = nullptr;
	//wvpデータ
	Matrix4x4* wvpData_ = nullptr;
	//マテリアル
	Vector4* materialData_ = nullptr;
	//SRT
	Transforms transform_{};

	int32_t myNumber_;
};

