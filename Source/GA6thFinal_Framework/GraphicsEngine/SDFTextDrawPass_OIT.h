#pragma once
#include "UIPassBase_OIT.h"

class SDFTextDrawPass_OIT : public UIPassBase_OIT
{
public:
    SDFTextDrawPass_OIT(const std::vector<UINT>* instanceIDs);
    virtual ~SDFTextDrawPass_OIT();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Update(ID3D12GraphicsCommandList* commandList, const float deltaTime) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

private:
    FX<GE::VS::SDF_FONT_FR, GE::PS::SDF_FONT_OIT> _fxSDF;
};