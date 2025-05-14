#pragma once

// TODO rt, dsv에 대한 설계 이어가야함.
//  rt는 renderscene이 pooling 하고있을 예정이므로 pool에 있는것에서 현재 사용가능한 rt의 id를 가져와 거기서 부터
//  사용하는 방식으로 설계 예정.

class RenderScene;
class ShaderBuilder;
class RenderPass
{
public:
    RenderPass();
    virtual ~RenderPass();

public:
    void SetClearValue(const Color& clearColor, float depthClear = 1.f, UINT clearStencil = 0);
    void SetShader(std::shared_ptr<ShaderBuilder> shader);
    void SetPipelineState(ComPtr<ID3D12PipelineState> pso);
    void SetOwnerScene(RenderScene* owner);

public:
    /**
     * @brief 1. clear render target and depth stencil buffer
     *		  2. set render target, pso, root signautre
     * @param commandList
     */
    virtual void Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect);
    virtual void Begin(ID3D12GraphicsCommandList* commandList);
    virtual void End(ID3D12GraphicsCommandList* commandList);
    virtual void Draw(ID3D12GraphicsCommandList* commandList) {};

protected:
    D3D12_VIEWPORT              _viewPort;
    D3D12_RECT                  _sissorRect;
    Color                       _clearColor;
    float                       _clearDepth;
    UINT                        _clearStencil;
    // 기본적으로 한개의 쉐이더와 한개의 pso를 주지만 필요에 따라 알아서 상속받아서 더 사용해도 무방.
    std::shared_ptr<ShaderBuilder> _shader;
    ComPtr<ID3D12PipelineState> _pipelineState;
    ComPtr<ID3D12DescriptorHeap>   _srvDescriptorHeap;

    RenderScene*                _ownerScene;
};
