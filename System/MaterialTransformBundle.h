#pragma once

#include "Transform.h"
#include <TransformationMatrix.h>
struct Material;
class Camera;
struct ID3D12Resource;
struct Vector4;
struct ID3D12Device;


class MaterialTransformBundle {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="color">色</param>
	/// <param name="transform">ワールドトランスフォーム</param>
	/// <param name="device">デバイス</param>
	MaterialTransformBundle(Vector4& color, Transforms& transform, ID3D12Device* device);
	~MaterialTransformBundle();

	void PreDraw(const Camera& camera);

	void ImguiUpdate();

	/// <summary>
	/// トランスフォームを入れる
	/// </summary>
	/// <param name="material"></param>
	/// <param name="transform"></param>
	void SetTransform(Transforms& transform) {	transform_ = transform;	}

	void SetColor(Vector4& color);

	void SetIsEnableLighting(bool isEnable);

	void SetUvTransform(Transforms& uvTransform);

	ID3D12Resource* GetTransformationResource() { return transformationResource_; }
	ID3D12Resource* GetMaterialResource() { return materialResource_; }
private:
	ID3D12Resource* transformationResource_ = nullptr;
	ID3D12Resource* materialResource_ = nullptr;
	//wvpデータ
	TransformationMatrix* transformationData_ = nullptr;
	//SRT
	Transforms transform_{};
	//uvTransform
	Transforms uvTransform_{};
	//マテリアル
	Material* material_ = nullptr;

};

