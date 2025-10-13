#pragma once
#include "RenderPass.h"

class DXRComputeSkeletalMeshPass : public RenderPass
{
public:
    DXRComputeSkeletalMeshPass() = default;
    virtual ~DXRComputeSkeletalMeshPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

    void Dispatch(ID3D12GraphicsCommandList* commandList, MeshInfo meshInfo);

private:
    void InitShaderAndPSO();

private:
    ComputeFX<GE::CS::SKELETAL_SKINNING> _fx;
    std::vector<VertexBufferID>          _vertexBufferIDs;
};