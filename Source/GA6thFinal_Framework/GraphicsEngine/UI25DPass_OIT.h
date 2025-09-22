#pragma once
#include "UIPassBase.h"

class UI25DPass_OIT : public UIPassBase
{
public:
    UI25DPass_OIT(const std::vector<UINT>& instanceIDs);
    virtual ~UI25DPass_OIT();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

private:
    FX<GE::VS::UI_FR, GE::PS::UI> _fx;
};