#pragma once
#include "UIPassBase.h"

class UI2DPass : public UIPassBase
{
public:
    UI2DPass(const std::vector<UINT>& instanceIDs);
    virtual ~UI2DPass();

public:
    void Initialize(RenderScene* ownerScene) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    std::unique_ptr<Camera>             _2DCamera;
    std::unique_ptr<ConstantBufferView> _cameraBuffer;
};