#pragma once
#include "RenderPass.h"

class ShaderBuilder;
class BaseMesh;

class GBufferPass : public RenderPass
{
public:
    GBufferPass() = default;
    virtual ~GBufferPass();

public:
    void Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitShaderAndPSO();
    void DrawStaticTwoSidedMesh(ID3D12GraphicsCommandList* commandList);
    void DrawStaticMeshes(ID3D12GraphicsCommandList*                     commandList,
                          const std::vector<std::pair<BaseMesh*, UINT>>& meshes);
    void DrawSkeletalMeshes(ID3D12GraphicsCommandList*                     commandList,
                          const std::vector<std::pair<BaseMesh*, UINT>>& meshes);

private:
    enum MeshType
    {
        STATIC,
        SKELTAL
    };
 
    std::vector<std::shared_ptr<ShaderBuilder>> _shader;

    enum MeshRenderType
    {
        STATIC_TWO_SIDED,
        STATIC_ONE_SIDED,
        SKELTAL_TWO_SIDED,
        SKELTAL_ONE_SIDED,
    };
    std::vector<ComPtr<ID3D12PipelineState>> _psos;
};
