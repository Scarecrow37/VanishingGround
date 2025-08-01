#pragma once
#include "RenderModule.h"

class GaussianBlurModule : public RenderModule
{
public:
    enum GaussianBlurType { AXIS_X, AXIS_Y, TYPE_END };

public:
    GaussianBlurModule();
    virtual ~GaussianBlurModule();

public:
    void Initialize() override;
    void Execute(ID3D12GraphicsCommandList* commandList, D3D12_GPU_DESCRIPTOR_HANDLE input, RenderTarget* output, GaussianBlurType type);

private:
    std::unique_ptr<ShaderBuilder> _shaders[TYPE_END];
    ComPtr<ID3D12PipelineState>    _pipelineStates[TYPE_END];
    BaseMesh*                      _quadMesh{nullptr};
};