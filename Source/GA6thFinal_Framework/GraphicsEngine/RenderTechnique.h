#pragma once

class RenderPass;
class RenderTechnique
{
    friend class RenderPass;

public:
    RenderTechnique();
    virtual ~RenderTechnique() = default;

public:
    void         AddRenderPass(std::unique_ptr<RenderPass> pass);
    void         SetOwnerScene(RenderScene* scene) { _ownerScene = scene; }
    void         AddRenderPassDatas(std::string_view sceneName);
    virtual void Initialize(ID3D12GraphicsCommandList* commandList) = 0;
    virtual void Update(ID3D12GraphicsCommandList* commandList);
    virtual void Execute(ID3D12GraphicsCommandList* commandList);

public:
    template <typename T>
    T* GetRenderTechnique()
    {
        for (auto& pass : _ownerScene->_techniques)
        {
            T* pointer = dynamic_cast<T*>(pass.get());
            if (pointer)
            {
                return pointer;
            }
        }
        return nullptr;
    }

    template <typename T>
    T* GetRenderPass()
    {
        for (auto& pass : _renderPasses)
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
    std::vector<std::unique_ptr<RenderPass>> _renderPasses;
    RenderScene*                             _ownerScene;
};