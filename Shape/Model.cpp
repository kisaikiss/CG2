#include "Model.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include <MatrixCalculations.h>
#include <DirectXUtils.h>
#include "Camera.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
#pragma region 1.中で必要となる変数の宣言
	ModelData modelData;
	std::vector<Vector4> positions;	//位置
	std::vector<Vector3> normals;	//法線
	std::vector<Vector2> texcoords;	//テクスチャ座標
	std::string line;				//ファイルから読んだ1行を格納するもの
#pragma endregion

#pragma region 2.ファイルを開く
	std::string filePath = directoryPath + "/" + filename;
	std::ifstream file(filePath);
	assert(file.is_open());
#pragma endregion

#pragma region 3.実際にファイルを読み ModelDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;//先頭の識別子を読む

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.f - texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f") {
			VertexData triangle[3];
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');// /区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
			/*	VertexData vertex = { position,texcoord,normal };
				modelData.vertices.push_back(vertex);*/
				triangle[faceVertex] = { position,texcoord,normal };
			}
			//頂点を逆順で登録する
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib") {
			std::string materialFileName;
			s >> materialFileName;

			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFileName);
		}
	}
#pragma endregion

#pragma region 4.ModelDataを返す
	return modelData;
#pragma endregion

}

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
#pragma region 1.中で必要となる変数の宣言
	MaterialData materialData;
	std::string line;
#pragma endregion

#pragma region 2.ファイルを開く
	std::string filePath = directoryPath + "/" + filename;
	std::ifstream file(filePath);
	assert(file.is_open());
#pragma endregion

#pragma region 3.実際にファイルを読みMatrialDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "map_Kd") {
			std::string textureFileName;
			s >> textureFileName;

			materialData.textureFilePath = directoryPath + "/" + textureFileName;
		}
	}
#pragma endregion

	
	return materialData;
}

int32_t Model::modelNum = 1;

Model::Model(DirectXCommon* dxCommon, const std::string& directoryPath, const std::string& filename) {
	myNumber_ = modelNum;
	modelNum++;
	commandList_ = dxCommon->GetCommandList();
	device_ = dxCommon->GetDevice();
	textureSystem_ = dxCommon->GetTextureSystem();
	//モデル読み込み
	modelData_ = LoadObjFile(directoryPath, filename);
	// 頂点リソース
	vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * modelData_.vertices.size());
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

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
	//テクスチャ読み込み
	textureNum_ = textureSystem_->Lord(modelData_.material.textureFilePath);
}

Model::~Model() {
	vertexResource_->Release();
	transformationResource_->Release();
	indexResource_->Release();
	materialResource_->Release();
}

void Model::Update() {
	std::stringstream myNumString;
	myNumString << myNumber_;
	std::string name = "Model";
	name = name + myNumString.str();
	ImGui::Begin(name.c_str());
	ImGui::DragFloat3("position", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("scale", &transform_.scale.x, 0.01f);
	ImGui::ColorEdit3("color", &material_->color.x);
	ImGui::DragFloat3("uvPosition", &uvTransform_.translate.x, 0.01f);
	ImGui::DragFloat3("uvRotate", &uvTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("uvScale", &uvTransform_.scale.x, 0.01f);
	ImGui::End();
}

void Model::Draw(const Camera& camera) {
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, camera.GetVeiwProjectionMatrix());
	transformationData_->WVP = worldViewProjectionMatrix;
	transformationData_->World = worldMatrix;

	Matrix4x4 uvTransformMatrix = MakeAffineMatrix(uvTransform_.scale, uvTransform_.rotate, uvTransform_.translate);
	material_->uvTransform = uvTransformMatrix;

	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	//commandList_->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	//wvp用CBufferの場所を指定
	commandList_->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2,textureSystem_->GetTextureSrvHandleGpu(textureNum_));
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//commandList_->DrawIndexedInstanced(UINT(modelData_.vertices.size()), 1, 0, 0, 0);
	commandList_->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
}
