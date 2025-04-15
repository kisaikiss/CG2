#include "Sphere.h"
#include <cmath>
#include <numbers>
#include <string>
#include <sstream>

#include "DirectXUtils.h"
#include <MatrixCalculations.h>
#include "Camera.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

int32_t Sphere::sphereNum = 1;

Sphere::~Sphere() {
	vertexResource_->Release();
	wvpResource_->Release();
	materialResource_->Release();
}

void Sphere::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {
	myNumber_ = sphereNum;
	sphereNum++;
	commandList_ = commandList;
	device_ = device;
	numberOfVertex_ = kSubdivision * kSubdivision * 6;
	vertexResource_ = CreateBufferResource(device, sizeof(VertexData) * numberOfVertex_);
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは球の頂点数のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * numberOfVertex_;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	wvpResource_ = CreateBufferResource(device_, sizeof(Matrix4x4));
	//データを書き込む
	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列を書き込んでおく
	*wvpData_ = MakeIdentity4x4();
	//トランスフォーム
	transform_.scale = { 1.f,1.f,1.f };
	transform_.translate = {0.f,0.f,0.9f};
	// マテリアル用のリソースを作る。今回はcolor一つ分のサイズを用意
	materialResource_ = CreateBufferResource(device_, sizeof(Vector4));
	//マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//今回は白を書き込んでみる
	*materialData_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

void Sphere::Update() {
	std::stringstream myNumString;
	myNumString << myNumber_;
	std::string name = "Sphere";
	name = name + myNumString.str();
	ImGui::Begin(name.c_str());
	ImGui::DragFloat3("position", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("scale", &transform_.scale.x, 0.01f);
	ImGui::ColorEdit3("color", &materialData_->x);
	ImGui::End();

	const float pi = std::numbers::pi_v<float>;
	const float kLatEvery = pi / static_cast<float>(kSubdivision);//緯度分割1つ分の角度
	const float kLonEvery = (2.f * pi) / static_cast<float>(kSubdivision);//経度分割1つ分の角度


	//緯度の方向に分割 -π/2 ~ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -pi / 2.f + kLatEvery * latIndex;//現在の緯度]
		float latNext = lat + kLatEvery;
		//経度の方向に分割 0 ~ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			float lon = lonIndex * kLonEvery;//現在の経度
			float lonNext = lon + kLonEvery;
			float u = static_cast<float>(lonIndex) / static_cast<float>(kSubdivision);
			float v = 1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision);
			float uNext = u + 1 / static_cast<float>(kSubdivision);
			float vNext = v - 1 / static_cast<float>(kSubdivision);

			//a,b,c,dを求める
			VertexData a{};
			VertexData b{};
			VertexData c{};
			VertexData d{};

			a.position = {
				std::cos(lat) * std::cos(lon),
				std::sin(lat),
				std::cos(lat) * std::sin(lon),
				1.0f
			};
			a.texcoord = {
				u, v
			};

			b.position = {
				std::cos(latNext) * std::cos(lon),
				std::sin(latNext),
				std::cos(latNext) * std::sin(lon),
				1.0f
			};
			b.texcoord = {
				u, vNext
			};

			c.position = {
				std::cos(lat) * std::cos(lonNext),
				std::sin(lat),
				std::cos(lat) * std::sin(lonNext),
				1.0f
			};
			c.texcoord = {
				uNext, v
			};

			d.position = {
				std::cos(latNext) * std::cos(lonNext),
				std::sin(latNext),
				std::cos(latNext) * std::sin(lonNext),
				1.0f
			};
			d.texcoord = {
				uNext, vNext
			};

			vertexData_[start] = a;
			vertexData_[start + 1] = b;
			vertexData_[start + 2] = c;
			vertexData_[start + 3] = c;
			vertexData_[start + 4] = b;
			vertexData_[start + 5] = d;
		}
	}
}

void Sphere::Draw(const Camera& camera) {
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, camera.GetVeiwProjectionMatrix());
	*wvpData_ = worldViewProjectionMatrix;
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	//wvp用CBufferの場所を指定
	commandList_->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList_->DrawInstanced(numberOfVertex_, 1, 0, 0);
}
