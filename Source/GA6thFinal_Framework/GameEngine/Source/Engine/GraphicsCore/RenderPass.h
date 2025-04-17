#pragma once

// TODO rt, dsv에 대한 설계 이어가야함.
//  rt는 renderscene이 pooling 하고있을 예정이므로 pool에 있는것에서 현재 사용가능한 rt의 id를 가져와 거기서 부터
//  사용하는 방식으로 설계 예정.

class RenderScene;

class RenderPass
{
public:
    struct DescriptorSet
    {
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> RenderTargetViewsHandle{};
        D3D12_CPU_DESCRIPTOR_HANDLE              DepthStencilViewHandle = {};
        UINT                                     RenderTagetCount{};
        bool                                     UseDepthStencilView = false;
        std::vector<ComPtr<ID3D12Resource>>      RenderTargetResources{};
        ComPtr<ID3D12Resource>                   DepthStencilResource = nullptr;
        void CreateDescriptorSet(UINT renderTargetCount, D3D12_CPU_DESCRIPTOR_HANDLE depthstencilHandle,
                                 ComPtr<ID3D12Resource> depthStencilBuffer, RenderScene* ownerScene, bool UseDepth);
    };
    RenderPass();
    virtual ~RenderPass();

public:
    void SetDescriptors(const DescriptorSet& descriptors);
    void SetClearValue(const Color& clearColor, float depthClear = 1.f, UINT clearStencil = 0);
    void SetShader(std::shared_ptr<Shader> shader);
    void SetPipelineState(ComPtr<ID3D12PipelineState> pso);
    void SetOwnerScene(RenderScene* owner);

public:
    /**
     * @brief 1. clear render target and depth stencil buffer
     *		  2. set render target, pso, root signautre
     * @param commandList
     */
    virtual void Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect);
    virtual void Begin(ComPtr<ID3D12GraphicsCommandList> commandList);
    virtual void End(ComPtr<ID3D12GraphicsCommandList> commandList);
    virtual void Draw(ComPtr<ID3D12GraphicsCommandList> commandList) {};

protected:
    D3D12_VIEWPORT              _viewPort;
    D3D12_RECT                  _sissorRect;
    DescriptorSet               _descriptor;
    Color                       _clearColor;
    float                       _clearDepth;
    UINT                        _clearStencil;
    std::shared_ptr<Shader>     _shader;
    ComPtr<ID3D12PipelineState> _pipelineState;
    RenderScene*                _ownerScene;
};
