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
    virtual void Initialize(ID3D12GraphicsCommandList* commandList) = 0;
    virtual void Update(ID3D12GraphicsCommandList* commandList);
    virtual void Execute(ID3D12GraphicsCommandList* commandList);

protected:
    std::vector<std::unique_ptr<RenderPass>> _renderPasses;
    RenderScene*                             _ownerScene;
};