#pragma once
#include "RenderTechnique.h"

class UITechnique : public RenderTechnique
{
public:
    UITechnique();
    virtual ~UITechnique();

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
    void Execute(ID3D12GraphicsCommandList* commandList) override;

private:
    std::vector<UINT> _renderDatas[SPRITE_TYPE_END];
};