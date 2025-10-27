#pragma once
#include "UIPassBase_OIT.h"

class UI25DPass_OIT : public UIPassBase_OIT
{
public:
    UI25DPass_OIT(const std::vector<UINT>* instanceIDs);
    virtual ~UI25DPass_OIT();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
};