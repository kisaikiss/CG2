#include "Triangle.h"
#include <string>
#include <sstream>

#include "DirectXUtils.h"
#include <MatrixCalculations.h>
#include "Camera.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include <VectorCalculations.h>

int32_t Triangle::triangleNum = 1;

Triangle::~Triangle() {
	vertexResource_->Release();
	transformationResource_->Release();
	materialResource_->Release();
}

void Triangle::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {
	myNumber_ = triangleNum;
	triangleNum++;
	commandList_ = commandList;
	device_ = device;
	vertexResource_ = CreateBufferResource(device, sizeof(VertexData) * 3);
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは球の頂点数のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 3;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	//transformation用のリソースを作る。TransformationMatrix 1つ分のサイズを用意する
	transformationResource_ = CreateBufferResource(device_, sizeof(TransformationMatrix));
	//データを書き込む
	//書き込むためのアドレスを取得
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));
	//単位行列を書き込んでおく
	transformationData_->WVP = MakeIdentity4x4();
	transformationData_->World = MakeIdentity4x4();
	//トランスフォーム
	transform_.scale = { 1.f,1.f,1.f };
	transform_.translate = { 0.f,0.f,0.f };
	// マテリアル用のリソースを作る。今回はcolor一つ分のサイズを用意
	materialResource_ = CreateBufferResource(device_, sizeof(MaterialData));
	//マテリアルにデータを書き込む
	//マテリアルデータへ書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//今回は白を書き込んでみる
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//Lightingを無効にする
	materialData_->enableLighting = false;

	//頂点データへ書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	// 左下
	vertexData_[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.f,1.f };
	// 上
	vertexData_[1].position = { 0.0f,0.5f,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.5f,0.f };
	// 右下
	vertexData_[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.f,1.f };

	

	vertexData_->normal = { 0.0f, 0.0f,-1.0f };
}

void Triangle::Update() {
	std::stringstream myNumString;
	myNumString << myNumber_;
	std::string name = "Trinangle";
	name = name + myNumString.str();
	ImGui::Begin(name.c_str());
	ImGui::DragFloat3("position", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("scale", &transform_.scale.x, 0.01f);
	ImGui::ColorEdit3("color", &materialData_->color.x);
	ImGui::End();


}

void Triangle::Draw(const Camera& camera) {
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, camera.GetVeiwProjectionMatrix());
	transformationData_->World = worldMatrix;
	transformationData_->WVP = worldViewProjectionMatrix;
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	//マテリアルCBufferの場所を指定
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//transformation用CBufferの場所を指定
	commandList_->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList_->DrawInstanced(3, 1, 0, 0);
}

void Triangle::SetVertex(Vector3 vertex[3]) {
	for (uint32_t i = 0; i < 3; i++) {
		vertexData_[i].position.x = vertex[i].x;
		vertexData_[i].position.y = vertex[i].y;
		vertexData_[i].position.z = vertex[i].z;
	}

}
