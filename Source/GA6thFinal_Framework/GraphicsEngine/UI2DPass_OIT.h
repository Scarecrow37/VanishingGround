#pragma once
#include "UIPassBase_OIT.h"

class UI2DPass_OIT : public UIPassBase_OIT
{
public:
    UI2DPass_OIT(const std::vector<UINT>* instanceIDs);
    virtual ~UI2DPass_OIT();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
};