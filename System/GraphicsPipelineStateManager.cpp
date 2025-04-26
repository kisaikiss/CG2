#include "GraphicsPipelineStateManager.h"
#include <Log.h>
#include <cassert>

void GraphicsPipelineStateManager::Create(const std::string& key, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) {
	// PSOを生成
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	HRESULT hr = device_->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState));
	if (FAILED(hr)) {
		Logger::Log("Failed to create graphics pipeline state");
		assert(SUCCEEDED(hr));
	}
	// キャッシュに追加
	pipelineStates_[key] = pipelineState;
}

void GraphicsPipelineStateManager::Destroy(const std::string& key) {
	auto it = pipelineStates_.find(key);
	if (it != pipelineStates_.end()) {
		pipelineStates_.erase(it);
		Logger::Log("Pipeline state destroyed: " + key);
	} else {
		Logger::Log("Pipeline state not found: " + key);
	}
}

ID3D12PipelineState* GraphicsPipelineStateManager::GetPipelineState(const std::string& key) const {
	auto it = pipelineStates_.find(key);
	if (it != pipelineStates_.end()) {
		return it->second.Get();
	}
	Logger::Log("Pipeline state not found: " + key);
	return nullptr;
}