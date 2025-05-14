#include "TriangularPyramid.h"
#include "MaterialTransformBundle.h"
#include "Engine.h"
#include <DirectXUtils.h>
#include "VertexData.h"
#include "Camera.h"
#include "VectorCalculations.h"

TriangularPyramid::TriangularPyramid(Engine* engine) {
	commandList_ = engine->GetCommandList();
	textureSystem_ = engine->GetTextureSystem();
	graphicsPipelineStateManager_ = engine->GetGraphicsPipelineStateManager();

	textureNum_ = 0;

}

void TriangularPyramid::Initialize(Engine* engine) {

	ID3D12Device*  device_ = engine->GetDevice();

	// 頂点リソース
	vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * 4);
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * 4);
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	// 左下
	vertexData_[0].position = { -0.5f,-0.5f, -0.5f,1.0f };
	vertexData_[0].texcoord = { 0.f,1.f };

	// 上
	vertexData_[1].position = { 0.0f,0.5f,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.5f,0.f };
	// 右下
	vertexData_[2].position = { 0.5f,-0.5f, -0.5f,1.0f };
	vertexData_[2].texcoord = { 1.f,1.f };
	// 奥
	vertexData_[3].position = { 0.0f, -0.5f, 0.5f,1.0f };
	vertexData_[3].texcoord = { 0.5f,1.f };

	for (uint32_t i = 0; i < 4; i++) {
		Vector3 normal{};
		normal.x = vertexData_[i].position.x;
		normal.y = vertexData_[i].position.y;
		normal.z = vertexData_[i].position.z;

		vertexData_[i].normal = Normalize(normal);
	}

	//IndexResource
	indexResource_ = CreateBufferResource(device_, sizeof(uint32_t) * 12);
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 12;
	//format
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	indexData_[0] = 0;	indexData_[1] = 1;	indexData_[2] = 2;
	indexData_[3] = 2;	indexData_[4] = 1;	indexData_[5] = 3;
	indexData_[6] = 3;	indexData_[7] = 1;	indexData_[8] = 0;
	indexData_[9] = 0;	indexData_[10] = 2;	indexData_[11] = 3;
}

void TriangularPyramid::Finalize() {
	indexResource_->Release();
	vertexResource_->Release();
}

void TriangularPyramid::Draw(Camera* camera) {
	materialTransformResource->PreDraw(*camera);

	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	commandList_->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	//wvp用CBufferの場所を指定
	commandList_->SetGraphicsRootConstantBufferView(1, materialTransformResource->GetTransformationResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(0, materialTransformResource->GetMaterialResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, textureSystem_->GetTextureSrvHandleGpu(textureNum_));
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList_->DrawIndexedInstanced(12, 1, 0, 0, 0);
}


