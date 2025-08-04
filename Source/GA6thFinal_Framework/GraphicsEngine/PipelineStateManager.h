#pragma once

class PipelineStateManager
{
public:
    PipelineStateManager()  = default;
    ~PipelineStateManager() = default;

public:
    ComPtr<ID3D12PipelineState> GetPipelineState(const PipelineStateStream& stream);

private:
    void CreatePipelineState(const PipelineStateStream& stream);

private:
    std::unordered_map<PipelineStateStream, ComPtr<ID3D12PipelineState>> _pipelineStates;
};
