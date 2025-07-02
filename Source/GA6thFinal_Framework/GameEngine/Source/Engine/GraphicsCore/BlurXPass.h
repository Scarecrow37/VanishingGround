#pragma once
#include "RenderPass.h"

class BlurXPass : public RenderPass
{
public:
    BlurXPass();
    virtual ~BlurXPass();

public:
    void Initialize() override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
};