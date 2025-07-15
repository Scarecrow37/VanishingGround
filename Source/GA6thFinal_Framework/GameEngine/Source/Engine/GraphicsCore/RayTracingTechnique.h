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
    void InitDXRDrawStaticMeshPass();
    void InitGbufferPass();

private:

//private:
//    std::vector<Vertex> _unifiedVertices;
//    std::vector<
};
