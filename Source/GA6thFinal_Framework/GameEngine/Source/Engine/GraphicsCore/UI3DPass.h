#pragma once
#include "RenderPass.h"

class UI3DPass : public RenderPass
{
public:
    UI3DPass();
    virtual ~UI3DPass();

public:
    void Initialize(RenderScene* ownerScene) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
};