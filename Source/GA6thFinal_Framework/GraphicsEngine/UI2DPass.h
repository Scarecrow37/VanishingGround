#pragma once
#include "UIPassBase.h"

class UI2DPass : public UIPassBase
{
public:
    UI2DPass(const std::vector<UINT>* instanceIDs);
    virtual ~UI2DPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Update(ID3D12GraphicsCommandList* commandList, const float deltaTime) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

private:
    void DrawSprite(ID3D12GraphicsCommandList* commandList, UINT offset, UINT instanceCount);
    void DrawSDFText(ID3D12GraphicsCommandList* commandList, SDFTextRenderer* component, UINT instanceID);
};