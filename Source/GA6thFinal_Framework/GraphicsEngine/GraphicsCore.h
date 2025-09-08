#pragma once

class GraphicsCore
{
public:
    GraphicsCore() = default;
    ~GraphicsCore() = default;

public:
    class ParticleManager*      GetParticleManager() const;
    class ResourceManager*      GetResourceManager() const;
    std::shared_ptr<Camera>     GetCamera(std::string_view cameraName) const;
    D3D12_GPU_DESCRIPTOR_HANDLE GetRenderSceneImage(std::string_view renderSceneName) const;
    ID3D12DescriptorHeap*       GetShaderResourceDescriptorHeap() const;
    ID3D12Device*               GetDevice() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferHandle() const;
    ID3D12GraphicsCommandList*  GetCommandList() const;
    RenderPassProperties&       GetRenderPassProperties() const;
    class SceneTransitionCore*  GetSceneTransitionCore() const;

public:
    void SetCamera(std::string_view renderSceneName, std::shared_ptr<Camera> camera) const;
    void SetEnvironmentSkyBox(std::string_view renderSceneName, std::wstring_view filePath) const;
    void SetIBLSkyBox(std::string_view renderSceneName, std::wstring_view filePath) const;
    void SetCurrentScene(std::string_view sceneName) const;
    void SyncGlobalVariable();

public:
    void AddRenderScene(std::string_view sceneName, RenderTechniqueFlag flag) const;
    void RegisterComponent(Animator* component) const;
    void RegisterComponent(std::string_view renderSceneName, MeshRenderer* component) const;
    void RegisterComponent(std::string_view renderSceneName, SpriteRenderer* component) const;
    void RegisterComponent(std::string_view renderSceneName, FontRenderer* component) const;
    void RegisterComponent(std::string_view renderSceneName, Light* component) const;

public:
    void LoadResource(std::wstring_view filePath, MeshRenderer* component, const std::function<void()>& callback);
    void LoadResource(std::wstring_view filePath, SpriteRenderer* component) const;
    void LoadResource(std::wstring_view filePath, FontRenderer* component) const;
    void LoadTextureResource(std::wstring_view filePath, class ParticleEmitter* component) const;
    void LoadModelResource(std::wstring_view filePath, class ParticleEmitter* component) const;

public:
    void Initialize(HWND hwnd, UINT width, UINT height, FeatureLevel feature, bool isEditorMode);
    void UpdateAnimation(const float deltaTime) const;
    void Update(const float deltaTime);
    void Render() const;
    void Flip() const;
    void Finalize() const;

public:
    void             ResetEnvironmentSkyBox(std::string_view sceneName) const;
    void             ResetIBLSkyBox(std::string_view sceneName) const;
    void             OnResize(UINT width, UINT height) const;
    void XM_CALLCONV DebugDraw3D(std::string_view sceneName, const BoundingSphere& sphere, FXMVECTOR color = DirectX::Colors::White) const;
    void XM_CALLCONV DebugDraw3D(std::string_view sceneName, const BoundingOrientedBox& obb, FXMVECTOR color = DirectX::Colors::White) const;
    void XM_CALLCONV DebugDraw3D(std::string_view sceneName, const BoundingFrustum& frustum, FXMVECTOR color = DirectX::Colors::White) const;  
    void XM_CALLCONV DebugDraw3D(std::string_view sceneName, const BoundingBox& box, FXMVECTOR color = DirectX::Colors::White) const;
    void XM_CALLCONV DebugDraw3D(std::string_view sceneName, FXMVECTOR origin, FXMVECTOR majorAxis, FXMVECTOR minorAxis, GXMVECTOR color = DirectX::Colors::White) const;
    void XM_CALLCONV DebugDraw3D(std::string_view sceneName, FXMVECTOR origin, FXMVECTOR direction, bool normalize = true, FXMVECTOR color = DirectX::Colors::White) const;
    void XM_CALLCONV DebugDraw3D(std::string_view sceneName, FXMVECTOR position, FXMVECTOR direction, float range, float innerCone, float outerCone, FXMVECTOR color = DirectX::Colors::White) const;
    void XM_CALLCONV DebugDraw2D(std::string_view sceneName, FXMVECTOR pointA, FXMVECTOR pointB, FXMVECTOR pointC, GXMVECTOR pointD, HXMVECTOR color = DirectX::Colors::White) const;
    void XM_CALLCONV DebugDraw2D(std::string_view sceneName, FXMVECTOR pointA, FXMVECTOR pointB, FXMVECTOR color = DirectX::Colors::White) const;

private:
    class Device*                     _device;
    class Renderer*                   _renderer;
    class CommandController*          _commandController;
    class DXResourceManager*          _dxResourceManager;
    class MultiRenderTargetManager*   _multiRenderTargetManager;
    class ResourceManager*            _resourceManager;
    class ViewManager*                _viewManager;
    class AnimationCore*              _animationCore;
    class LightCore*                  _lightCore;
    class ParticleManager*            _particleManager;
    class DebugDrawCore*              _debugDrawCore;
    class RenderPassDatas*            _renderPassDatas;
    class ModuleManager*              _moduleManager;
    class PipelineStateManager*       _pipelineStateManager;
    class ThreadPool*                 _threadPool;
    class SceneTransitionCore*        _sceneTransitionCore;

private:
    std::queue<std::function<void()>> _resourceLoadQueue;
};