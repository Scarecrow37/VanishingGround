#pragma once
#include "RenderPass.h"

class BaseMesh;
class GBufferPass : public RenderPass
{
    enum MeshType { STATIC, SKELTAL, END };    
    enum MeshRenderType { STATIC_TWO_SIDED, STATIC_ONE_SIDED, SKELTAL_TWO_SIDED, SKELTAL_ONE_SIDED, };

public:
    GBufferPass() = default;
    virtual ~GBufferPass();

public:
    void Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitShaderAndPSO();
    void DrawMeshes(ID3D12GraphicsCommandList* commandList, const std::vector<MeshRenderer*>& meshes, MeshType type);

private:
    std::vector<std::unique_ptr<ShaderBuilder>>                   _shaders;
    std::vector<ComPtr<ID3D12PipelineState>>                      _psos;
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, GBuffer::GBUFFER_END> _gBufferHandles;
};
