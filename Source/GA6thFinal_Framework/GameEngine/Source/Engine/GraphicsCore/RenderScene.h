#pragma once

class RenderTarget;
class RenderPass;
class RenderTechnique;
class FrameResource;
class Quad;
class Shader;
// 임시 오브젝트
class TempObject;
class RenderScene
{
public:
    RenderScene();
    ~RenderScene() = default;

public:
    void UpdateRenderScene();

public:
    void InitializeRenderScene(UINT renderTargetCount);
    void RegisterOnRenderQueue(MeshRenderer* renderable);
    // Set Decriptor랑 Set OnwerPass는 해줌.
    void AddRenderTechnique(const std::string& name, std::shared_ptr<RenderTechnique> technique);
    void Excute(ComPtr<ID3D12GraphicsCommandList> commandList);
    UINT GetCurrentRendertarget() { return _currnetRederTarget; }
    void RenderOnBackBuffer(ComPtr<ID3D12GraphicsCommandList> commandList);
    void RenderOnEditor(ComPtr<ID3D12GraphicsCommandList> commandList);

private:
    void CreateRenderTargetPool(UINT renderTargetCount);
    void CreateFrameDepthStencil();
    void CreateMSAARenderTarget();
    void CreatePso();
    void CreateDescriptorHeap();
    void CopyMSAATexture(ComPtr<ID3D12GraphicsCommandList> commandList);

public:

    std::vector<std::shared_ptr<RenderTarget>> _renderTargetPool;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>   _renderTargetHandles;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>   _renderTargetSRVHandles;
    UINT                                       _currnetRederTarget = 0;


    UINT _currentFrameIndex=0;


    // 250425
public:
    void InitializeRenderScene();

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

public:
    // 가지고있는 technique들
    std::unordered_map<std::string, std::shared_ptr<RenderTechnique>> _techniques;

    // 0: albedo, 1: normal ,2:ORM , 3:emissive, 4: depth, 5: costom depth(bit mask,후처리용)
    UINT                              _gbufferCount = 6;
    std::vector<std::shared_ptr<RenderTarget>> _gBuffer;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> _gBufferSrvHandles;

    // 후처리시 사용할 rt pool 혹은 각 테크별로 돌려서 쓸?
    UINT _renderTargetPoolCount = 3;
    std::vector<std::shared_ptr<RenderTarget>> _renderTargets;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>  _renderTargetSrvHandles;

    // 후처리시 필요한 사실상 통상적 메쉬 그리기가 이루어진, 음영처리가 완성된 타겟 하나
    std::shared_ptr<RenderTarget> _meshLightingTarget;
    D3D12_CPU_DESCRIPTOR_HANDLE _meshLightingSrv;

    // frame단위로 사용하는 depth stencil buffer
    D3D12_CPU_DESCRIPTOR_HANDLE _depthStencilHandle;
    ComPtr<ID3D12Resource>      _depthStencilBuffer;

    // 렌더링할 목록
    std::vector<MeshRenderer*>                  _renderQueue;
    
    //frame resource와 카메라 리소스.
    std::vector<std::shared_ptr<FrameResource>> _frameResources;
    ComPtr<ID3D12Resource>                      _cameraBuffer;

private:
    std::unique_ptr<Quad>        _frameQuad;
    std::unique_ptr<Shader>      _frameShader;
    ComPtr<ID3D12PipelineState>  _framePSO;
    ComPtr<ID3D12DescriptorHeap> _srvDescriptorHeap;

    // 폐기 목록? msaa
private:
    ComPtr<ID3D12Resource> _nonMSAATexture;
    D3D12_CPU_DESCRIPTOR_HANDLE _nonMSAARtHandle;
    D3D12_CPU_DESCRIPTOR_HANDLE _nonMSAASrvHandle;
};
