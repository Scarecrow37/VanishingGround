#pragma once
#include "UIPassBase.h"

class UI3DPass : public UIPassBase
{
public:
    UI3DPass(const std::vector<UINT>* instanceIDs);
    virtual ~UI3DPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;    
};