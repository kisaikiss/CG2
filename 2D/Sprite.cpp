#include "Sprite.h"
#include <MatrixCalculations.h>
#include <DirectXUtils.h>
#include <string>
#include <sstream>
#include "Engine.h"


Sprite::Sprite(Engine* engine, const std::string& fileName) {
	if (engine == nullptr) {
		assert(0);
	}

	commandList_ = engine->GetCommandList();
	device_ = engine->GetDevice();
	textureSystem_ = engine->GetTextureSystem();
	textureNum_ = textureSystem_->Lord(fileName);
	size_ = textureSystem_->GetTextureSize(textureNum_);
	// 頂点リソース
	vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * 4);
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	float width = size_.x;
	float height = size_.y;
	// 左下
	vertexData_[0].position = { 0.0f,height,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.f,1.0f };
	// 左上
	vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	// 右下
	vertexData_[2].position = { width,height,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.f,1.f };
	// 右上
	vertexData_[3].position = { width,0.0f,0.0f,1.0f };
	vertexData_[3].texcoord = { 1.f,0.0f };

	for (uint32_t i = 0; i < 4; i++) {
		vertexData_[i].normal = { 0.0f,0.0f,-1.0f };
	}

	//IndexResource
	indexResource_ = CreateBufferResource(device_, sizeof(uint32_t) * 6);
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	//format
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	indexData_[0] = 0;	indexData_[1] = 1;	indexData_[2] = 2;
	indexData_[3] = 1;	indexData_[4] = 3;	indexData_[5] = 2;

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
	materialResource_ = CreateBufferResource(device_, sizeof(Material));
	//マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&material_));
	//今回は白を書き込んでみる
	material_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//Lightingを無効にする
	material_->enableLighting = false;
	//単位行列を書き込んでおく
	material_->uvTransform = MakeIdentity4x4();
}

Sprite::~Sprite() {
	vertexResource_->Release();
	transformationResource_->Release();
	indexResource_->Release();
	materialResource_->Release();
}

void Sprite::Update() {

	std::stringstream myNumString;
	myNumString << textureNum_;
	std::string name = "Sprite";
	name = name + myNumString.str();
	ImGui::Begin(name.c_str());
	ImGui::DragFloat2("position", &position_.x, 1.0f);
	ImGui::DragFloat2("size", &size_.x, 1.0f);
	ImGui::ColorEdit4("color", &material_->color.x);
	ImGui::End();
	SetSize(size_.x, size_.y);
	SetPosition(position_);
}

void Sprite::Draw() {
	transform_.translate.x = position_.x;
	transform_.translate.y = position_.y;

	commandList_->SetGraphicsRootDescriptorTable(2, textureSystem_->GetTextureSrvHandleGpu(textureNum_));
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	commandList_->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//Sprite用のWorldViewProjectionMatrixを作る
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, static_cast<float>(WinApp::kWindowWidth), static_cast<float>(WinApp::kWindowHeight), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrixSprite));
	transformationData_->WVP = worldViewProjectionMatrixSprite;
	//wvp用CBufferの場所を指定
	commandList_->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());
	//描画! (DrawCall)。
	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::SetPosition(const Vector2& position) {
	position_ = position;
}

void Sprite::SetSize(float width, float height) {
	// 左下
	vertexData_[0].position = { 0.0f,height,0.0f,1.0f };
	// 左上
	vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };
	// 右下
	vertexData_[2].position = { width,height,0.0f,1.0f };
	// 右上
	vertexData_[3].position = { width,0.0f,0.0f,1.0f };
}
