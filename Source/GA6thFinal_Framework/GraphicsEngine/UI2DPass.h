#pragma once
#include "UIPassBase.h"

class UI2DPass : public UIPassBase
{
public:
    UI2DPass(const std::vector<UINT>* instanceIDs);
    virtual ~UI2DPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
};