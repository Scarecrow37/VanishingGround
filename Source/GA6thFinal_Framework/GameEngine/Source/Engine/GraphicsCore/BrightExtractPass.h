#pragma once
#include "RenderPass.h"

class RenderTarget;
class BrightExtractPass : public RenderPass
{
public:
    BrightExtractPass();
    virtual ~BrightExtractPass();

public:
    void Initialize() override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    RenderTarget* _renderTarget{nullptr};
};