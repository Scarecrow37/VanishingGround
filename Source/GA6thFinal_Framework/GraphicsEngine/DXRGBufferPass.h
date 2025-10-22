#pragma once
#include "RenderPass.h"

class BaseMesh;
class DXRGBufferPass : public RenderPass
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
    DXRGBufferPass() = default;
    virtual ~DXRGBufferPass() = default;

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void AddRenderPassDatas(std::string_view sceneName) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
    void Update(ID3D12GraphicsCommandList* commandList, const float deltaTime) override;

private:
    void InitShaderAndPSO();
    void DrawMeshes(ID3D12GraphicsCommandList* commandList, MeshType meshType, Material::BlendModeType blendModeType,
                    CullMode cullMode, UINT offset);

private:
    std::vector<MeshInfo*> _mesheInfos[MeshType::MESH_TYPE_END][Material::BlendModeType::BMT_END - 1][CullMode::END];
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, DXRGBuffer::DXRGBUFFER_END> _gBufferHandles;
    ComPtr<ID3D12PipelineState> _psos[MeshType::MESH_TYPE_END][Material::BlendModeType::BMT_END - 1][CullMode::END];

    std::vector<InstanceData>         _instanceDatas;
    std::unique_ptr<StructuredBuffer> _instanceDatasBuffer;

    FX<GE::VS::STATIC_FR, GE::PS::DXRGBUFFER>   _fxStaticMesh;
    FX<GE::VS::SKELETAL_FR, GE::PS::DXRGBUFFER> _fxSkeletalMesh;

    // Debug
    SharedResource<RenderTarget> _gBufferRenderTargets[3];
};