#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <map>


class GraphicsPipelineStateManager {
public:
	GraphicsPipelineStateManager(ID3D12Device* device) : device_(device) {}
	~GraphicsPipelineStateManager() = default;

	/// <summary>
	/// PSOを生成
	/// </summary>
	/// <param name="key">PSOを区別するための名前</param>
	/// <param name="desc">PSOの設定</param>
	void Create(const std::string& key, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);

	/// <summary>
	/// PSOを解放
	/// </summary>
	/// <param name="key">PSOを区別するための名前</param>
	void Destroy(const std::string& key);

	ID3D12PipelineState* GetPipelineState(const std::string& key) const;
private:
	//デバイス
	ID3D12Device* device_ = nullptr;
	//PSOのキャッシュ
	std::map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> pipelineStates_;
};

