#pragma once
#include "RenderPass.h"

class ShadowMapPass : public RenderPass
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
    // 초기화 헬퍼 함수
    void CreateShadowMapResource();
    void CreateShaderAndPSO();

    // 매 프레임 실행되는 핵심 로직
    void UpdateCascades(const Vector3& lightDirection);
    void DrawMeshes(ID3D12GraphicsCommandList* commandList, int shaderType, MeshType meshType, int cascadedIndex);

private:
    FX<GE::VS::STATIC_SHADOW_FR, GE::PS::SHADOW>   _fxStaticShadow;
    FX<GE::VS::SKELETAL_SHADOW_FR, GE::PS::SHADOW> _fxSkeletalShadow;

    // 그림자 맵 리소스
    ComPtr<ID3D12Resource>                   _shadowMap;
    D3D12_CPU_DESCRIPTOR_HANDLE              _shadowMapDSVs[MAX_CASCADES];
    DescriptorHandles                        _shadowMapSRV;
    std::unique_ptr<ConstantBufferView>      _cascadeDataCBV;
    std::vector<ComPtr<ID3D12PipelineState>> _psos;
    std::vector<RenderData>                  _renderDatas[MeshType::END];

    // 캐스케이드 관련 데이터
    CascadeData    _cascadeData;
    D3D12_VIEWPORT _viewport;
    D3D12_RECT     _scissorRect;
    UINT           _shadowMapSize = 2048;

    // 디버그용
    DescriptorHandles _debugHandles[MAX_CASCADES];
};