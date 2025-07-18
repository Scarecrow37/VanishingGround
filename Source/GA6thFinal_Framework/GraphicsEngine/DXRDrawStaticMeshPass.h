#pragma once
#include "RenderPass.h"

class RTPipelineBuilder;

class DXRDrawStaticMeshPass : public RenderPass
{
public:
    DXRDrawStaticMeshPass() = default;
    virtual ~DXRDrawStaticMeshPass();

public:
    void Initialize(RenderScene* ownerScene, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    void CreateStateObject();
    void CreateShaderTable();
    void CreateShaderResource();
    void UpdateStaticMeshVIBufferID(ID3D12GraphicsCommandList* commandList);

    void WriteCommand();

private:
    bool                               _init = false;
    ComPtr<ID3D12StateObject> _pso;
    ComPtr<ID3D12RootSignature> _globalRootsignature;
    ComPtr<ID3D12Resource>      _shaderTable;
    uint32_t                    _shaderTableEntrySize = 0;
    SharedResource<UnorderedAccessView>_outputResourceUAV;

    std::vector<VertexBufferID> _vertexBufferIDs;
    std::vector<IndexBufferID>  _indexBufferIDs;
};
