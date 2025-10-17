#pragma once
#include "PointLightShadowAtlas.h"
#include "RenderPass.h"

class PointLightShadowPass : public RenderPass
{
    enum CullMode
    {
        CULL_BACK,
        CULL_FRONT,
        TWO_SIDED,
        END
    };

public:
    PointLightShadowPass();
    virtual ~PointLightShadowPass();

public:
    D3D12_GPU_DESCRIPTOR_HANDLE GetShadowAtlasSRV() const { return _shadowAtlasSRV.GPU; }

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                    ID3D12GraphicsCommandList* commandList) override;
    void AddRenderPassDatas(std::string_view sceneName) override;
    void Update(ID3D12GraphicsCommandList* commandList, const float deltaTime) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    void CreateShadowCubeMapResource();
    void CreateShaderAndPSO();
    void UpdateShadowLights();
    void UpdateCubeFaceMatrices(UINT lightIndex, const Vector3& lightPosition, float lightRange);

    void DrawMeshes(ID3D12GraphicsCommandList* commandList, MeshType meshType, CullMode cullMode, UINT lightIndex,
                     UINT instanceOffset);
    
private:
    ComPtr<ID3D12PipelineState> _psos[MeshType::MESH_TYPE_END][CullMode::END];
    std::vector<MeshInfo*>      _meshInfos[MeshType::MESH_TYPE_END][CullMode::END];
    std::vector<MeshInfo*>      _perLightMeshInfos[MAX_SHADOW_POINT_LIGHT][MeshType::MESH_TYPE_END][CullMode::END];
    std::vector<InstanceData>   _instanceDatas;
    std::unique_ptr<StructuredBuffer> _instanceDatasBuffer;

    FX<GE::VS::STATIC_POINT_LIGHT_SHADOW_FR, GE::PS::POINT_LIGHT_SHADOW>   _fxStaticMesh;
    FX<GE::VS::SKELETAL_POINT_LIGHT_SHADOW_FR, GE::PS::POINT_LIGHT_SHADOW> _fxSkeletalMesh;

    PointLightShadowAtlas _atlas;
    std::vector<UINT>     _activeLightIndices;

    DescriptorHandles _shadowAtlasSRV;

    struct PointLightShadowData
    {
        Matrix  ViewProjection[6];
        Vector3 LightPosition;
        float   FarPlane;
    };
    std::unique_ptr<ConstantBufferView> _pointLightShadowDataCBV;
    std::array<Matrix, 6>               _cubeFaceViewProjections[MAX_SHADOW_POINT_LIGHT];

    UINT _shadowFaceSize = 1024;
    UINT _alignedSize    = 0;
};
