#pragma once

class Shader;
class FrameResource;
class MeshRenderer;
class Renderer
{
    enum class ResterizeMode
    {
        SOLID,
        WIREFRAME,
        END
    };

public:
    Renderer();
    ~Renderer();

public:
    void RegisterRenderQueue(MeshRenderer* component);

public:
    void Initialize();
    void Update();
    void Render();

private:
    HRESULT CreatePipelineState();

    // 임시
    void UpdateFrameResource();

private:
    std::vector<MeshRenderer*>     _components;
    std::unique_ptr<Shader>        _shader;
    ComPtr<ID3D12PipelineState>    _pipelineState[2];
    std::unique_ptr<FrameResource> _frameResource;
    ComPtr<ID3D12Resource>         _cameraBuffer;
    UINT                           _currnetState;
};