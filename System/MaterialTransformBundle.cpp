#include "MaterialTransformBundle.h"
#include <MatrixCalculations.h>
#include <DirectXUtils.h>
#include <Camera.h>
#include <d3d12.h>
#include <Material.h>

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"


MaterialTransformBundle::MaterialTransformBundle(Vector4& color, Transforms& transform, ID3D12Device* device) {
	//transformation用のリソースを作る。TransformationMatrix 1つ分のサイズを用意する
	transformationResource_ = CreateBufferResource(device, sizeof(TransformationMatrix));
	//データを書き込む
	//書き込むためのアドレスを取得
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));
	//単位行列を書き込んでおく
	transformationData_->WVP = MakeIdentity4x4();
	transformationData_->World = MakeIdentity4x4();
	//トランスフォーム
	transform_ = transform;
	// マテリアル用のリソースを作る。今回はcolor一つ分のサイズを用意
	materialResource_ = CreateBufferResource(device, sizeof(Material));
	//マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&material_));
	//今回は白を書き込んでみる
	material_->color = color;
	//Lightingを有効にする
	material_->enableLighting = true;
	//単位行列を書き込んでおく
	material_->uvTransform = MakeIdentity4x4();
	//uvTransform
	uvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

}

MaterialTransformBundle::~MaterialTransformBundle() {
	transformationResource_->Release();
	materialResource_->Release();
}

void MaterialTransformBundle::PreDraw(const Camera& camera) {
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, camera.GetVeiwProjectionMatrix());
	transformationData_->WVP = worldViewProjectionMatrix;
	transformationData_->World = worldMatrix;

	Matrix4x4 uvTransformMatrix = MakeAffineMatrix(uvTransform_.scale, uvTransform_.rotate, uvTransform_.translate);
}

void MaterialTransformBundle::ImguiUpdate() {
	ImGui::Begin("materialTransform");
	ImGui::DragFloat3("position", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("scale", &transform_.scale.x, 0.01f);
	ImGui::ColorEdit4("color", &material_->color.x);
	ImGui::DragFloat3("uvPosition", &uvTransform_.translate.x, 0.01f);
	ImGui::DragFloat3("uvRotate", &uvTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("uvScale", &uvTransform_.scale.x, 0.01f);
	ImGui::End();
}

void MaterialTransformBundle::SetColor(Vector4& color) { material_->color = color; }

void MaterialTransformBundle::SetIsEnableLighting(bool isEnable) { material_->enableLighting = isEnable; }

void MaterialTransformBundle::SetUvTransform(Transforms& uvTransform) {
	material_->uvTransform = MakeAffineMatrix(uvTransform.scale, uvTransform.rotate, uvTransform.translate);
}