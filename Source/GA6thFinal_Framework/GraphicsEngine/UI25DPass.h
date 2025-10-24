#pragma once
#include "UIPassBase.h"

class UI25DPass : public UIPassBase
{
public:
    UI25DPass(const std::vector<UINT>* instanceIDs);
    virtual ~UI25DPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;    
};