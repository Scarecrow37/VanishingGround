#pragma once
#include "RenderPass.h"

class UI2DPass : public RenderPass
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
    const std::vector<UINT>&            _instanceIDs;
    std::unique_ptr<Camera>             _2DCamera;
    std::unique_ptr<ConstantBufferView> _cameraBuffer;
    std::unique_ptr<ConstantBufferView> _instnaceIDBuffer;
};