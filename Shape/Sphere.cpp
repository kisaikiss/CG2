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
	transformationResource_->Release();
	indexResource_->Release();
	materialResource_->Release();
}

void Sphere::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, TextureSystem* textureSystem) {
	myNumber_ = sphereNum;
	sphereNum++;
	commandList_ = commandList;
	device_ = device;
	textureSystem_ = textureSystem;
	numberOfVertex_ = kSubdivision * kSubdivision * 4;
	numberOfIndex_ = kSubdivision * kSubdivision * 6;

	//頂点リソース
	vertexResource_ = CreateBufferResource(device, sizeof(VertexData) * numberOfVertex_);
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは球の頂点数のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * numberOfVertex_;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//IndexResource
	indexResource_ = CreateBufferResource(device, sizeof(uint32_t) * numberOfIndex_);
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * numberOfIndex_;
	//format
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

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
	transform_.translate = {0.f,0.f,0.9f};
	// マテリアル用のリソースを作る。今回はcolor一つ分のサイズを用意
	materialResource_ = CreateBufferResource(device_, sizeof(MaterialData));
	//マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//今回は白を書き込んでみる
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//Lightingを有効にする
	materialData_->enableLighting = true;
	//テクスチャ読み込み
	uvChackTextureNum_ = textureSystem_->Lord("resources/uvChecker.png");
	monsterBallTextureNum_ = textureSystem_->Lord("resources/monsterBall.png");
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
	ImGui::ColorEdit3("color", &materialData_->color.x);
	ImGui::Checkbox("useMonsterBall", &useMonsterBall_);
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
			uint32_t vertexStart = (latIndex * kSubdivision + lonIndex) * 4;
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
			a.normal.x = a.position.x;
			a.normal.y = a.position.y;
			a.normal.z = a.position.z;

			b.position = {
				std::cos(latNext) * std::cos(lon),
				std::sin(latNext),
				std::cos(latNext) * std::sin(lon),
				1.0f
			};
			b.texcoord = {
				u, vNext
			};
			b.normal.x = b.position.x;
			b.normal.y = b.position.y;
			b.normal.z = b.position.z;

			c.position = {
				std::cos(lat) * std::cos(lonNext),
				std::sin(lat),
				std::cos(lat) * std::sin(lonNext),
				1.0f
			};
			c.texcoord = {
				uNext, v
			};
			c.normal.x = c.position.x;
			c.normal.y = c.position.y;
			c.normal.z = c.position.z;

			d.position = {
				std::cos(latNext) * std::cos(lonNext),
				std::sin(latNext),
				std::cos(latNext) * std::sin(lonNext),
				1.0f
			};
			d.texcoord = {
				uNext, vNext
			};
			d.normal.x = d.position.x;
			d.normal.y = d.position.y;
			d.normal.z = d.position.z;

			vertexData_[vertexStart] = a;
			vertexData_[vertexStart + 1] = b;
			vertexData_[vertexStart + 2] = c;
			vertexData_[vertexStart + 3] = d;

			indexData_[start] = vertexStart;	indexData_[start + 1] = vertexStart + 1;	indexData_[start + 2] = vertexStart + 2;
			indexData_[start + 3] = vertexStart + 1;	indexData_[start + 4] = vertexStart + 3;	indexData_[start + 5] = vertexStart + 2;
		}
	}
}

void Sphere::Draw(const Camera& camera) {
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, camera.GetVeiwProjectionMatrix());
	transformationData_->WVP = worldViewProjectionMatrix;
	transformationData_->World = worldMatrix;
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	commandList_->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	//wvp用CBufferの場所を指定
	commandList_->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, useMonsterBall_ ? textureSystem_->GetTextureSrvHandleGpu(monsterBallTextureNum_) : textureSystem_->GetTextureSrvHandleGpu(uvChackTextureNum_));
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList_->DrawIndexedInstanced(numberOfIndex_, 1, 0, 0, 0);
}
