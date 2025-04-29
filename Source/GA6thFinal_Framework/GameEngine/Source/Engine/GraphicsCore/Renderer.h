#pragma once

class Shader;
class FrameResource;
class MeshRenderer;
class RenderScene;
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
    void RegisterRenderQueue(bool** isActive, MeshRenderer* component,std::string_view sceneName);

public:
    void Initialize();
    void Update();
    void Render();
    void Flip();
    D3D12_GPU_DESCRIPTOR_HANDLE GetRenderSceneImage(std::string_view renderSceneName);
    std::shared_ptr<Camera>     GetCamera(std::string_view renderSceneName);

public:
    //imgui 관련 함수
    void InitializeImgui();
    void PreUnInitializeImgui();
    void ImguiBegin();
    void ImguiEnd();

private:
    D3D12_GPU_DESCRIPTOR_HANDLE SceneView(RenderScene* scene);

private:
    // imgui 전용 descriptor heap
    ComPtr<ID3D12DescriptorHeap>                                  _imguiDescriptorHeap = nullptr;

private:
    std::vector<std::pair<bool, MeshRenderer*>>                   _components;
    std::unordered_map<std::string, std::shared_ptr<RenderScene>> _renderScenes;
    UINT                                                          _currnetState;
};