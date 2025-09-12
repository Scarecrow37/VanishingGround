#pragma once
#include "RenderPass.h"

class BaseMesh;
class GBufferPass : public RenderPass
{    
    enum CullMode
    {
        CULL_BACK,
        CULL_FRONT,
        TWO_SIDED,
        END
    };

    struct RenderData
    {
        BaseMesh* mesh;
        UINT      instanceID;
        UINT      customDepth;
    };

public:
    GBufferPass() = default;
    virtual ~GBufferPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void AddRenderPassDatas(std::string_view sceneName) override;
    void Update(ID3D12GraphicsCommandList* commadList, const float deltaTime) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitShaderAndPSO();
    void DrawMeshes(ID3D12GraphicsCommandList* commandList, MeshType meshType, Material::BlendModeType blendModeType, CullMode cullMode);

private:
    ComPtr<ID3D12PipelineState> _psos[MeshType::MESH_TYPE_END][Material::BlendModeType::BMT_END][CullMode::END];
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, GBuffer::GBUFFER_END> _gBufferHandles;
    std::vector<RenderData> _renderDatas[MeshType::MESH_TYPE_END][Material::BlendModeType::BMT_END][CullMode::END];

    SharedResource<RenderTarget> _gBufferRenderTargets[4];

    FX<GE::VS::STATIC_FR, GE::PS::GBUFFER>   _fxStaticMesh;
    FX<GE::VS::SKELETAL_FR, GE::PS::GBUFFER> _fxSkeletalMesh;
};