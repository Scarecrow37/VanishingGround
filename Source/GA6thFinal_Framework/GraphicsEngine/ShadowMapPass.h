//#pragma once
//#include "RenderPass.h"
//
//// 최대 캐스케이드 레벨 정의 (pch.h나 Constants.h로 이동 가능)
//#ifndef MAX_CASCADES
//#define MAX_CASCADES 4
//#endif
//
//class ShadowMapPass : public RenderPass
//{
//public:
//    ShadowMapPass();
//    virtual ~ShadowMapPass();
//
//    // RenderPass 인터페이스 구현
//    void Initialize(RenderScene* ownerScene, ID3D12GraphicsCommandList* commandList) override;
//    void Begin(ID3D12GraphicsCommandList* commandList) override;
//    void Draw(ID3D12GraphicsCommandList* commandList) override;
//    void End(ID3D12GraphicsCommandList* commandList) override;
//
//    // 외부에서 그림자 맵 리소스에 접근하기 위한 Getter
//    ID3D12Resource*             GetShadowMapResource() const { return m_shadowMap.Get(); }
//    D3D12_GPU_DESCRIPTOR_HANDLE GetShadowMapSrv() const { return m_shadowMapSrv.GPU; }
//
//private:
//    // 초기화 헬퍼 함수
//    void CreateShadowMapResource();
//    void CreateShaderAndPSO();
//
//    // 매 프레임 실행되는 핵심 로직
//    void UpdateCascades(const Camera& mainCamera, const Vector3& lightDirection);
//    void RenderMeshes(ID3D12GraphicsCommandList* commandList, int cascadeIndex);
//
//private:
//    // 그림자 맵 리소스
//    ComPtr<ID3D12Resource> m_shadowMap; // Texture2DArray
//    DescriptorHandles      m_shadowMapSrv;
//    DescriptorHandles      m_shadowMapDsvs[MAX_CASCADES];
//
//    // 렌더링에 필요한 PSO 및 셰이더
//    ComPtr<ID3D12PipelineState>    m_pso;
//    std::unique_ptr<ShaderBuilder> m_shader;
//
//    // 캐스케이드 관련 데이터
//    D3D12_VIEWPORT m_viewport;
//    D3D12_RECT     m_scissorRect;
//    UINT           m_shadowMapSize = 2048; // 그림자 맵 해상도
//
//    // 매 프레임 계산되어 LightData에 전달될 행렬들
//    Matrix m_lightView;
//    Matrix m_lightProj[MAX_CASCADES];
//    float  m_cascadeSplits[MAX_CASCADES];
//};