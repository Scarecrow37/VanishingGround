#pragma once
#include "RenderTechnique.h"

class RayTracingTechnique : public RenderTechnique
{
public:
    RayTracingTechnique();
    virtual ~RayTracingTechnique();

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
    void Execute(ID3D12GraphicsCommandList* commandList) override;
private:
    void InitDXRDrawMeshPass(ID3D12GraphicsCommandList* commandList);
    void InitDXRGbufferPass(ID3D12GraphicsCommandList* commandList);
    void InitDXRComputeSkeletalMeshPass(ID3D12GraphicsCommandList* commandList);
};
