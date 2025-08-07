#pragma once
#include "RenderModule.h"

class GaussianBlurModule : public RenderModule
{
public:
    enum BlurType { AXIS_X, AXIS_Y, TYPE_END };

public:
    GaussianBlurModule();
    virtual ~GaussianBlurModule();

public:
    void Initialize() override;
    void Execute(ID3D12GraphicsCommandList* commandList, D3D12_GPU_DESCRIPTOR_HANDLE input, RenderTarget* output, DXGI_FORMAT rtvFormat, BlurType type, UINT mipLevel = 0);
    void Execute(ID3D12GraphicsCommandList* commandList, RenderTarget* input, RenderTarget* output, DXGI_FORMAT rtvFormat, BlurType type, UINT mipLevel = 0);

private:
    void InitShaderAndPipelineState();

private:
    PipelineStateStream                      _pipelineStates[TYPE_END];
    FX<GE::VS::QUAD, GE::PS::GAUSSIANBLUR_X> _fxBlurX;
    FX<GE::VS::QUAD, GE::PS::GAUSSIANBLUR_Y> _fxBlurY;
    BaseMesh*                                _quadMesh{nullptr};
};