#pragma once

class RenderPass
{
public:
    RenderPass();
    virtual ~RenderPass();

public:
    virtual void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList);
    virtual void AddRenderPassDatas(std::string_view sceneName) {};
    virtual void Update(ID3D12GraphicsCommandList* commadList) {};
    virtual void Begin(ID3D12GraphicsCommandList* commandList) {};
    virtual void Draw(ID3D12GraphicsCommandList* commandList) {};
    virtual void End(ID3D12GraphicsCommandList* commandList) {};

protected:
    template<typename T>
    T* GetRenderPass()
    {
        for (auto& pass : _ownerTechnique->_renderPasses)
        {
            T* pointer = dynamic_cast<T*>(pass.get());
            if (pointer)
            {
                return pointer;
            }
        }
        return nullptr;
    }

protected:
    RenderScene* _ownerScene{nullptr};
    RenderTechnique* _ownerTechnique{nullptr};

    std::unique_ptr<ShaderBuilder> _shader;
    ComPtr<ID3D12PipelineState>    _pipelineState;

    SharedResource<RenderTarget> _meshRenderTarget;
    SharedResource<RenderTarget> _finalRenderTarget;
};
