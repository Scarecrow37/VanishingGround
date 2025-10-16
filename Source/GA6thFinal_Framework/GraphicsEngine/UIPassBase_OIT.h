#pragma once
#include "RenderPass.h"

class UIPassBase_OIT : public RenderPass
{
public:
    UIPassBase_OIT();
    UIPassBase_OIT(const std::vector<UINT>* instanceIDs);
    virtual ~UIPassBase_OIT();

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
    UnorderedAccessView*              _headBuffer{nullptr};
    UnorderedAccessView*              _nodesBuffer{nullptr};
    UnorderedAccessView*              _atomicCounterBuffer{nullptr};

    FX<GE::VS::UI_FR, GE::PS::UI_OIT> _fx;
    DepthStencilView*                 _depthStencilView{nullptr};
    ConstantBufferView*               _cameraBuffer{nullptr};
};