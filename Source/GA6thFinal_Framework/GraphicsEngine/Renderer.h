#pragma once

class Shader;
class FrameResource;
class MeshRenderer;
class RendererFileEvent;
class Resource;
class SpriteRenderer;
class TextRenderer;
class GraphicsBase;
class Renderer
{
public:
    Renderer();
    ~Renderer();

public:
    D3D12_GPU_DESCRIPTOR_HANDLE GetRenderSceneImage(std::string_view renderSceneName);
    std::shared_ptr<Camera>     GetCamera(std::string_view renderSceneName);
    RenderScene*                GetRenderScene(std::string_view renderSceneName);

public:
    void SetCamera(std::string_view renderSceneName, std::shared_ptr<Camera> camera);
    void SetCurrentScene(std::string_view sceneName);
    void SetEnvironmentSkyBox(std::string_view renderSceneName, std::wstring_view filePath) const;
    void SetIBLSkyBox(std::string_view renderSceneName, std::wstring_view filePath) const;

public:
    void AddToBeReleasedComponent(GraphicsBase* component) { _toBeReleasedComponents.push_back(component); }
    void AddRenderScene(std::string_view sceneName, RenderTechniqueFlag flag);
    void RegisterRenderQueue(std::string_view sceneName, MeshRenderer* component);
    void RegisterRenderQueue(std::string_view sceneName, SpriteRenderer* component);
    void RegisterRenderQueue(std::string_view sceneName, TextRenderer* component);
    void RegisterRenderQueue(std::string_view sceneName, SDFTextRenderer* component);

public:
    void ResetEnvironmentSkyBox(std::string_view sceneName);
    void ResetIBLSkyBox(std::string_view sceneName);
    void UpdateRenderQueue();
    void ClearComponents();
    void ClearRenderQueue();

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
    std::vector<GraphicsBase*>                                    _toBeReleasedComponents;

    // Scene To BackBuffer
    FX<GE::VS::QUAD, GE::PS::TO_BACKBUFFER> _fx;
    ComPtr<ID3D12PipelineState>             _pipelineState;
    BaseMesh*                               _frameQuad;
};