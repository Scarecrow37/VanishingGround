#pragma once

class RenderPass;
class RenderScene;
class RenderTechnique
{
public:
    enum RenderTechniqueType
    {
        MESH_LIGHTING,
        SHADOW,
        END
    };

public:
    RenderTechnique();
    virtual ~RenderTechnique() = default;

public:
    void         SetTechniqueType(RenderTechniqueType type) { _type = type; }
    void         AddRenderPass(std::shared_ptr<RenderPass> pass);
    void         SetOwnerScene(RenderScene* scene) { _ownerScene = scene; }
    virtual void Initalize();
    virtual void Execute(ComPtr<ID3D12GraphicsCommandList> commadList);

protected:
    std::vector<std::shared_ptr<RenderPass>> _renderPasses;
    RenderScene*                             _ownerScene;
    RenderTechniqueType                      _type;
};
