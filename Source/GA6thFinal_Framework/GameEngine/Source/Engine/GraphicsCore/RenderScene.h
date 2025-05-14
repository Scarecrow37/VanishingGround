#pragma once

class RenderTarget;
class RenderPass;
class RenderTechnique;
class FrameResource;
class Quad;
class ShaderBuilder;
class Camera;
class MeshRenderer;
class RenderScene
{
public:
    enum GBuffer
    {
        BASECOLOR,
        NORMAL,
        ORM,
        EMISSIVE,
        WORLDPOSITION,
        DEPTH,
        CUSTOMDEPTH,
        END
    };

public:
    RenderScene();
    ~RenderScene();

public:
    void                    UpdateRenderScene();
    std::shared_ptr<Camera> GetCamera() { return _camera; }

public:
    void RenderOnBackBuffer(ID3D12GraphicsCommandList* commandList);
    void RenderOnEditor(ID3D12GraphicsCommandList* commandList);

    // 렌더신 시작시 한번만 호출
    void InitializeRenderScene();
    // 렌더할 메쉬 등록
    void RegisterOnRenderQueue(MeshRenderer* component);
    // 렌더큐에서 삭제된건?? 물어봐야함.

    // 렌더 기술 등록
    void AddRenderTechnique(std::shared_ptr<RenderTechnique> technique);
    // 실행
    void Execute(ID3D12GraphicsCommandList* commandList);

    // Scene view 용 최종 렌더 이미지 얻기
    D3D12_CPU_DESCRIPTOR_HANDLE GetFinalImage();

private:
    // 사용할 gbuffer와 render target pool 생성
    void CreateRenderTarget();
    // 한 frame에 사용할 depth stencil 생성하기(1개)->늘어나야하는가?
    // 각 테크 별로 필요하면 만들어쓰기. 이거는 사용하지 않도록
    void CreateDepthStencil();
    // 화면에 찍어낼 용도의 quad mesh와 frame shader 만들기.
    void CreateFrameQuadAndFrameShader();
    // frame에서 사용할 pso 만들어주기
    void CreateFramePSO();
    // frame에서 사용할 SRV descriptor Heap 만들어주기.
    void CreateSrvDescriptorHeap();
    // frame Resource Backbuffer 갯수만큼 생성해주기.
    void CreateFrameResource();
    // scene이 제공해주는 카메라 만들기
    void CreateCamera();

public:
    UINT _currentFrameIndex = 0;
    // 가지고있는 technique들
    std::vector<std::shared_ptr<RenderTechnique>> _techniques;

    // 0: basecolor, 1: normal ,2:ORM , 3:emissive, 4:world position, 5: depth, 6: custom depth(bit mask,후처리용)
    UINT                                       _gBufferCount = GBuffer::END;
    std::vector<std::shared_ptr<RenderTarget>> _gBuffer;

    // 후처리시 사용할 rt pool 혹은 각 테크별로 돌려서 쓸?
    UINT                                       _renderTargetPoolCount = 3;
    std::vector<std::shared_ptr<RenderTarget>> _renderTargets;

    // 후처리시 필요한 사실상 통상적 메쉬 그리기가 이루어진, 음영처리가 완성된 타겟 하나
    std::shared_ptr<RenderTarget> _meshLightingTarget;

    // frame단위로 사용하는 depth stencil buffer
    D3D12_CPU_DESCRIPTOR_HANDLE _depthStencilHandle;
    ComPtr<ID3D12Resource>      _depthStencilBuffer;

    // 렌더링할 목록
    std::vector<MeshRenderer*> _renderQueue;

    // frame resource와 카메라 리소스.
    std::vector<std::shared_ptr<FrameResource>> _frameResources;
    ComPtr<ID3D12Resource>                      _cameraBuffer;

    // 카메라 한개
    std::shared_ptr<Camera> _camera;
    // 화면 크기 quad
    std::unique_ptr<Quad> _frameQuad;

private:
    std::unique_ptr<ShaderBuilder> _frameShader;
    ComPtr<ID3D12PipelineState>    _framePSO;
    ComPtr<ID3D12DescriptorHeap>   _srvDescriptorHeap;

    // 폐기 목록? msaa
private:
    ComPtr<ID3D12Resource>      _nonMSAATexture;
    D3D12_CPU_DESCRIPTOR_HANDLE _nonMSAARtHandle;
    D3D12_CPU_DESCRIPTOR_HANDLE _nonMSAASrvHandle;
};
