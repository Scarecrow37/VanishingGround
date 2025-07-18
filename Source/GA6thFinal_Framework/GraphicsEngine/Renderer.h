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
    D3D12_GPU_DESCRIPTOR_HANDLE GetRenderSceneImage(std::string_view renderSceneName);
    std::shared_ptr<Camera>     GetCamera(std::string_view renderSceneName);
    RenderScene*                GetRenderScene(std::string_view renderSceneName);

public:
    void SetCamera(std::string_view renderSceneName, std::shared_ptr<Camera> camera);

public:
    void AddRenderScene(std::string_view sceneName, RenderTechniqueFlag flag);
    void RegisterRenderQueue(std::string_view sceneName, MeshRenderer* component);
    void RegisterRenderQueue(std::string_view sceneName, SpriteRenderer* component);
    void RegisterRenderQueue(std::string_view sceneName, FontRenderer* component);

    void SetSkyBox(std::string_view sceneName, std::wstring_view path);
    void ResetSkyBox(std::string_view sceneName);

public:
    void Initialize();
    void Update();
    void Render();
    void Flip();

private:
    void CreateDefaultResource();
    void CreateDefaultGeometry();
    void CreateDefaultTexture();
    void CreateDefaultRenderTarget();

private:
    std::unordered_map<std::string, std::unique_ptr<RenderScene>> _renderScenes;
    std::list<std::shared_ptr<Resource>>                          _defaultResource;

public:
    bool _isRaytracing = true;
    //bool _isRaytracing = false;
};