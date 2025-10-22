#pragma once
#include "UIPassBase_OIT.h"

class UI2DPass_OIT : public UIPassBase_OIT
{
public:
    UI2DPass_OIT(const std::vector<UINT>* instanceIDs);
    virtual ~UI2DPass_OIT();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

private:
    FX<GE::VS::UI_FR, GE::PS::UI_OIT> _fx;
    DepthStencilView*                 _depthStencilView{nullptr};
    ConstantBufferView*               _cameraBuffer{nullptr};
};