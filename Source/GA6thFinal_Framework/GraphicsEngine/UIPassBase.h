#pragma once
#include "RenderPass.h"

class UIPassBase : public RenderPass
{
public:
    UIPassBase();
    UIPassBase(const std::vector<UINT>* instanceIDs);
    virtual ~UIPassBase();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

protected:
    void UpdateBuffer(ID3D12GraphicsCommandList* commandList);

protected:
    std::shared_ptr<Model>            _quadModel;
    std::unique_ptr<StructuredBuffer> _instanceIDBuffer;
    const std::vector<UINT>*          _instanceIDs{nullptr};
    BaseMesh*                         _halfQuad{nullptr};

    FX<GE::VS::UI_FR, GE::PS::UI> _fx;
    DepthStencilView*             _depthStencilView{nullptr};
    ConstantBufferView*           _cameraBuffer{nullptr};
};