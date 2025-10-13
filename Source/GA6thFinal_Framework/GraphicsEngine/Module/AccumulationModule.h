#pragma once
#include "RenderModule.h"

class AccumulationModule : public RenderModule
{
public:
    AccumulationModule();
    virtual ~AccumulationModule();

public:
    void Initialize() override;
    void Execute(ID3D12GraphicsCommandList* commandList, D3D12_GPU_DESCRIPTOR_HANDLE input, UnorderedAccessView* output);

private:
    FX<GE::VS::QUAD, GE::PS::ACCUMULATION> _fx;
    ComPtr<ID3D12PipelineState>            _pipelineState;
    BaseMesh*                              _quadMesh{nullptr};
};