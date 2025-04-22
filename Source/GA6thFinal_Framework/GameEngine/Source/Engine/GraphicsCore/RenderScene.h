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

private:
    void CreatePso();
    void CreateDescriptorHeap();
    void CopyMSAATexture(ComPtr<ID3D12GraphicsCommandList> commandList);

public:
    std::vector<MeshRenderer*>                  _renderQueue;
    std::vector<std::shared_ptr<FrameResource>> _frameResources;
    ComPtr<ID3D12Resource>                      _cameraBuffer;

public:
    std::unordered_map<std::string, std::shared_ptr<RenderTechnique>> _techniques;

    std::vector<std::shared_ptr<RenderTarget>> _renderTargetPool;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>   _renderTargetHandles;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>   _renderTargetSRVHandles;
    UINT                                       _currnetRederTarget = 0;

    D3D12_CPU_DESCRIPTOR_HANDLE _depthStencilHandle;
    ComPtr<ID3D12Resource>      _depthStencilBuffer;

    UINT _currentFrameIndex=0;

    std::unique_ptr<Quad>        _frameQuad;
    ComPtr<ID3D12PipelineState>  _framePSO;
    ComPtr<ID3D12DescriptorHeap> _srvDescriptorHeap;

    // 임시
    std::unique_ptr<Shader> _frameShader;

private:
    ComPtr<ID3D12Resource> _nonMSAATexture;
    D3D12_CPU_DESCRIPTOR_HANDLE _nonMSAARtHandle;
    D3D12_CPU_DESCRIPTOR_HANDLE _nonMSAASrvHandle;
};
