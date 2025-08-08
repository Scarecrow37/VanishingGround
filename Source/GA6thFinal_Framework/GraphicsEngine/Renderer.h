#pragma once

class Shader;
class FrameResource;
class MeshRenderer;
class RendererFileEvent;
class Resource;
class SpriteRenderer;
class FontRenderer;
class Renderer
{
public:
    Renderer();
    ~Renderer();

public:
    D3D12_GPU_DESCRIPTOR_HANDLE GetRenderSceneImage(std::string_view renderSceneName);
    std::shared_ptr<Camera>     GetCamera(std::string_view renderSceneName);
    RenderScene*                GetRenderScene(std::string_view renderSceneName);
    const float                       GetTotalTime() const { return _totalTime; }

public:
    void SetCamera(std::string_view renderSceneName, std::shared_ptr<Camera> camera);
    void SetCurrentScene(std::string_view sceneName);

public:
    void AddRenderScene(std::string_view sceneName, RenderTechniqueFlag flag);
    void RegisterRenderQueue(std::string_view sceneName, MeshRenderer* component);
    void RegisterRenderQueue(std::string_view sceneName, SpriteRenderer* component);
    void RegisterRenderQueue(std::string_view sceneName, FontRenderer* component);

    void SetSkyBox(std::string_view sceneName, std::wstring_view path);
    void ResetSkyBox(std::string_view sceneName);

public:
    void Initialize();
    void Update(const float deltaTime);
    void Render();
    void Flip();

private:
    void RenderToBackBuffer();

private:
    void CreateDefaultResource();
    void CreateDefaultGeometry();
    void CreateDefaultTexture();
    void CreateDefaultRenderTarget();
    void CreateDefaultShader();

private:
    std::unordered_map<std::string, std::unique_ptr<RenderScene>> _renderScenes;
    std::list<std::shared_ptr<Resource>>                          _defaultResource;
    std::string_view                                              _currentSceneName;

    // Scene To BackBuffer
    ComPtr<ID3D12PipelineState>                                   _pipelineState;
    std::unique_ptr<ShaderBuilder>                                _shader;
    BaseMesh*                                                     _frameQuad;

    float                                                         _totalTime;
};