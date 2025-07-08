#pragma once

class Shader;
class FrameResource;
class MeshRenderer;
class RenderScene;
class RendererFileEvent;
class Resource;
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
    void RegisterRenderQueue(std::string_view sceneName, MeshRenderer* component);

    // 에디터나 게임씬 말고 다른 에디터 뷰에서 스카이 박스를 띄우고싶을수도 있으니 함수 오버로딩.
    void SetSkyBox(std::string_view sceneName, std::string_view path);
    void SetSkyBox(std::string_view path);
    void ResetSkyBox();
    void ResetSkyBox(std::string_view sceneName);

public:
    void Initialize();
    void Update();
    void Render();
    void Flip();

public:
    // imgui 관련 함수
    void InitializeImgui();
    void PreUnInitializeImgui();
    void ImguiBegin();
    void ImguiEnd();

private:
    void CreateDefaultResource();
    void CreateDefaultGeometry();
    void CreateDefaultTexture();
    void CreateDefaultRenderTarget();

private:
    std::unique_ptr<RendererFileEvent> _rendererFileEvent;

private:
    std::vector<std::pair<bool, MeshRenderer*>>                   _components;
    std::unordered_map<std::string, std::unique_ptr<RenderScene>> _renderScenes;
    UINT                                                          _currnetState;
    std::list<std::shared_ptr<Resource>>                          _defaultResource;

public:
    bool _isRaytracing = true;
};