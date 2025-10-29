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
    FX<GE::VS::UI_FR, GE::PS::UI>             _fx;
    FX<GE::VS::SDF_FONT_FR, GE::PS::SDF_FONT> _fxSDF;
    ComPtr<ID3D12PipelineState>               _sdfPipelineState;

    std::shared_ptr<Model>                    _quadModel;
    std::unique_ptr<StructuredBuffer>         _instanceIDBuffer;
    const std::vector<UINT>*                  _instanceIDs{nullptr};
    BaseMesh*                                 _halfQuad{nullptr};
    DepthStencilView*                         _depthStencilView{nullptr};
    ConstantBufferView*                       _cameraBuffer{nullptr};
};