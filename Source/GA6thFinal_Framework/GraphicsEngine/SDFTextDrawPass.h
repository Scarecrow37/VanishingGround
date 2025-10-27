#pragma once
#include "UIPassBase.h"

class SDFTextDrawPass : public UIPassBase
{
public:
    SDFTextDrawPass(const std::vector<UINT>* instanceIDs);
    virtual ~SDFTextDrawPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Update(ID3D12GraphicsCommandList* commandList, const float deltaTime) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

private:
    FX<GE::VS::SDF_FONT_FR, GE::PS::SDF_FONT> _fxSDF;
};