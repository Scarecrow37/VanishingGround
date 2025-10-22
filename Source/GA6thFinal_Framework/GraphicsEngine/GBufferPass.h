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

public:
    GBufferPass() = default;
    virtual ~GBufferPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void AddRenderPassDatas(std::string_view sceneName) override;
    void Update(ID3D12GraphicsCommandList* commandList, const float deltaTime) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitShaderAndPSO();
    void DrawMeshes(ID3D12GraphicsCommandList* commandList, MeshType meshType, Material::BlendModeType blendModeType, CullMode cullMode, UINT offset);

private:
    std::vector<MeshInfo*> _mesheInfos[MeshType::MESH_TYPE_END][Material::BlendModeType::BMT_END - 1][CullMode::END];
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, GBuffer::GBUFFER_END> _gBufferHandles;
    ComPtr<ID3D12PipelineState> _psos[MeshType::MESH_TYPE_END][Material::BlendModeType::BMT_END - 1][CullMode::END];
    
    std::vector<InstanceData>         _instanceDatas;
    std::unique_ptr<StructuredBuffer> _instanceDatasBuffer;

    FX<GE::VS::STATIC_FR, GE::PS::GBUFFER>   _fxStaticMesh;
    FX<GE::VS::SKELETAL_FR, GE::PS::GBUFFER> _fxSkeletalMesh;

    // Debug
    SharedResource<RenderTarget> _gBufferRenderTargets[4];
};