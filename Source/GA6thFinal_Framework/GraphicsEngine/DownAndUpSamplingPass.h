#pragma once
#include "RenderPass.h"

class DownAndUpSamplingPass : public RenderPass
{
    enum { DOWN_SAMPLING, UP_SAMPLING, SAMPLING_END};

public:
    DownAndUpSamplingPass();
    virtual ~DownAndUpSamplingPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void AddRenderPassDatas(std::string_view sceneName) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    std::vector<D3D12_GPU_DESCRIPTOR_HANDLE>  _activeSRVs;
    SharedResource<RenderTarget>              _pingpongTarget[2];
    ComPtr<ID3D12PipelineState>               _pipelineStates[SAMPLING_END];
    std::unique_ptr<ShaderBuilder>            _shaders[SAMPLING_END];

    ComPtr<ID3D12Resource> _finalTexture;
    DescriptorHandles      _finalHandle;
};