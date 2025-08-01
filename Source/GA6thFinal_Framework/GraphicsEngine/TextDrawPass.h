#pragma once
#include "RenderPass.h"

class TextDrawPass : public RenderPass
{
public:
    TextDrawPass(SpriteBatch* spriteBatch);
    virtual ~TextDrawPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    SpriteBatch* _spriteBatch;
};