#pragma once
#include "RenderPass.h"

class RTPipelineBuilder;

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
    void                      CreateStateObject();
    D3D12_ROOT_SIGNATURE_DESC CreateRayGenRootDest();
    D3D12_ROOT_SIGNATURE_DESC CreateHitRootDesc();

private:
    ComPtr<ID3D12StateObject> _pso;
    ComPtr<ID3D12RootSignature> _emptyRootsignature;
    std::vector<MaterialID>     _staticMeshMaterialID;
};
