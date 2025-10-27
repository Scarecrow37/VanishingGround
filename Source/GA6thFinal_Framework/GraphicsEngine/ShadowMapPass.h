#pragma once
#include "RenderPass.h"

constexpr UINT MAX_SHADOW_MAP = MAX_CASCADES + 1;

class ShadowMapPass : public RenderPass
{
    enum CullMode
    {
        CULL_BACK,
        CULL_FRONT,
        TWO_SIDED,
        END
    };

public:
    ShadowMapPass();
    virtual ~ShadowMapPass();

public:
    D3D12_GPU_DESCRIPTOR_HANDLE GetShadowMapSRV() const { return _shadowMapSRV.GPU; }
    D3D12_GPU_VIRTUAL_ADDRESS   GetCascadeDataCBV() const { return _cascadeDataCBV->GetGPUVirtualAddress(); }

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void AddRenderPassDatas(std::string_view sceneName) override;
    void Update(ID3D12GraphicsCommandList* commandList, const float deltaTime) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    void CreateShadowMapResource();
    void CreateShaderAndPSO();

    void UpdateCascades(const Vector3& lightDirection);
    void UpdateSkeletalShadow(const Vector3& lightDirection);
    void DrawMeshes(ID3D12GraphicsCommandList* commandList, MeshType type, CullMode cullMode, UINT offset);

private:
    ComPtr<ID3D12PipelineState>                    _psos[MeshType::MESH_TYPE_END][CullMode::END];
    std::vector<MeshInfo*>                         _meshInfos[MeshType::MESH_TYPE_END][CullMode::END];
    std::vector<InstanceData>                      _instanceDatas;
    std::unique_ptr<StructuredBuffer>              _instanceDatasBuffer;

    FX<GE::VS::STATIC_SHADOW_FR, GE::PS::SHADOW>   _fxStaticMesh;
    FX<GE::VS::SKELETAL_SHADOW_FR, GE::PS::SHADOW> _fxSkeletalMesh;

    // 캐스케이드 그림자 맵 리소스
    ComPtr<ID3D12Resource>                   _shadowMap;
    D3D12_CPU_DESCRIPTOR_HANDLE              _shadowMapDSV;
    DescriptorHandles                        _shadowMapSRV;
    std::unique_ptr<ConstantBufferView>      _cascadeDataCBV;

    // 캐스케이드 관련 데이터
    CascadeData    _cascadeData;
    D3D12_VIEWPORT _viewport;
    D3D12_RECT     _scissorRect;
    UINT           _shadowMapSize = 2048;

    // 디버그용
    DescriptorHandles _debugHandles[MAX_SHADOW_MAP];
};