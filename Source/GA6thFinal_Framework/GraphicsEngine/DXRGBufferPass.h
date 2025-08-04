#pragma once
#include "RenderPass.h"

class BaseMesh;
class DXRGBufferPass : public RenderPass
{
    enum MeshType
    {
        STATIC_CULL_BACK,
        STATIC_CULL_FRONT,
        STATIC_TWO_SIDED,
        SKELETAL_CULL_BACK,
        SKELETAL_CULL_FRONT,
        SKELETAL_TWO_SIDED,
        END
    };
    struct RenderData
    {
        BaseMesh* mesh;
        UINT      instanceID;
        UINT      customDepth;
    };

public:
    DXRGBufferPass() = default;
    virtual ~DXRGBufferPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
    void Update(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitShaderAndPSO();
    void DrawMeshes(ID3D12GraphicsCommandList* commandList, int shaderType, MeshType meshType);

private:
    std::vector<std::unique_ptr<ShaderBuilder>>                   _shaders;
    std::vector<ComPtr<ID3D12PipelineState>>                      _psos;
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, GBuffer::GBUFFER_END> _gBufferHandles;
    std::vector<RenderData>                                       _renderDatas[MeshType::END];

    FX<GE::VS::STATIC_FR, GE::PS::DXRGBUFFER> _staticMeshFX;
    FX<GE::VS::SKELETAL_FR, GE::PS::DXRGBUFFER> _skeletalMeshFX;
};