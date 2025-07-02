#pragma once
#include "RenderPass.h"

class DXRDrawStaticMeshPass : public RenderPass
{
public:
    DXRDrawStaticMeshPass() = default;
    virtual ~DXRDrawStaticMeshPass();

public:
    void Initialize() override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    void CreateStaticAS(ID3D12GraphicsCommandList* commandList);
};
