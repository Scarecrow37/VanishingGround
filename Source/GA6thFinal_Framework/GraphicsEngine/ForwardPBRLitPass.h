#pragma once
#include "RenderPass.h"

class ForwardPBRLitPass : public RenderPass
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
        BaseMesh*     Mesh;
        InstanceData* InstanceData;
    };
    
public:
    ForwardPBRLitPass() = default;
    virtual ~ForwardPBRLitPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Update(ID3D12GraphicsCommandList* commandList, const float deltaTime) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    void DrawMeshes(ID3D12GraphicsCommandList* commandList, MeshType meshType, CullMode cullMode);

private:
    ComPtr<ID3D12PipelineState> _psos[MeshType::MESH_TYPE_END][CullMode::END];
    std::vector<RenderData>     _renderDatas[MeshType::MESH_TYPE_END][CullMode::END];

    FX<GE::VS::STATIC_FORWARD_FR, GE::PS::FORWARD_PBR_LIGHTING> _fxStaticMesh;
    FX<GE::VS::SKELETAL_FORWARD_FR, GE::PS::FORWARD_PBR_LIGHTING> _fxSkeletalMesh;
};
