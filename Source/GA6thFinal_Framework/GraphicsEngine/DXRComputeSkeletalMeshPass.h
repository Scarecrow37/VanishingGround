#pragma once
#include "RenderPass.h"

class DXRComputeSkeletalMeshPass : public RenderPass
{
public:
    DXRComputeSkeletalMeshPass() = default;
    virtual ~DXRComputeSkeletalMeshPass();

public:
    void Initialize(RenderScene* ownerScene, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

    void Dispatch(ID3D12GraphicsCommandList* commandList, class MeshRenderer* renderer);

private:
    void InitShaderAndPSO();

private:
    std::unique_ptr<class ShaderBuilder> _shader;
    ComPtr<ID3D12PipelineState>          _pso;
};