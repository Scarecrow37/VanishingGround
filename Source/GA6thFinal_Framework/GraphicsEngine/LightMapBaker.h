#pragma once

class LightMapBaker
{
public:
    LightMapBaker();
    ~LightMapBaker();

public:
    void Initialize(RenderScene* ownerScene);
    void StartBaking();

private:
    void BakeLightMap();

private:
    std::thread                    _bakeThread;
    RenderScene*                   _ownerScene{nullptr};
    std::unique_ptr<ShaderBuilder> _shader;
    ComPtr<ID3D12PipelineState>    _pipelineState;
    std::condition_variable        _conditionVariable;
    std::mutex                     _mutex;
    bool                           _isBaking{false};
};