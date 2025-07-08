#pragma once
#include "RenderPass.h"
class ParticleBrightExtractPass : public RenderPass
{
public:
    ParticleBrightExtractPass();
    virtual ~ParticleBrightExtractPass();

public:
    void Initialize() override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    RenderTarget* _renderTarget{nullptr};
};
