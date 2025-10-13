#pragma once

class PipelineStateManager
{
public:
    PipelineStateManager()  = default;
    ~PipelineStateManager() = default;

public:
    ComPtr<ID3D12PipelineState> GetPipelineState(const PipelineStateStream& stream);
    ComPtr<ID3D12PipelineState> GetPipelineState(const ComputePipelineStateStream& stream);

private:
    void CreatePipelineState(const PipelineStateStream& stream);
    void CreatePipelineState(const ComputePipelineStateStream& stream);

private:
    std::unordered_map<PipelineStateStream, ComPtr<ID3D12PipelineState>>        _pipelineStates;
    std::unordered_map<ComputePipelineStateStream, ComPtr<ID3D12PipelineState>> _computePipelineStates;
};
