#pragma once
#include "RenderPass.h"

class UI2DPass : public RenderPass
{
public:
    UI2DPass();
    virtual ~UI2DPass();

public:
    void Initialize() override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    std::unique_ptr<Camera>             _2DCamera;
    std::unique_ptr<ConstantBufferView> _cameraBuffer;
};