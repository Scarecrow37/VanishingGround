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

public:
    void SetCamera(std::string_view renderSceneName, std::shared_ptr<Camera> camera);
    void SetSkyBox(std::string_view renderSceneName, std::wstring_view filePath);
    void SetCurrentScene(std::string_view sceneName);

public:
    void AddRenderScene(std::string_view sceneName, RenderTechniqueFlag flag);
    void RegisterComponent(Animator* component);
    void RegisterComponent(std::string_view renderSceneName, MeshRenderer* component);
    void RegisterComponent(std::string_view renderSceneName, SpriteRenderer* component);
    void RegisterComponent(std::string_view renderSceneName, FontRenderer* component);
    void RegisterComponent(std::string_view renderSceneName, Light* component);

public:
    void LoadResource(std::wstring_view filePath, MeshRenderer* component);
    void LoadResource(std::wstring_view filePath, SpriteRenderer* component);
    void LoadResource(std::wstring_view filePath, FontRenderer* component);
    void LoadTextureResource(std::wstring_view filePath, class ParticleEmitter* component);
    void LoadModelResource(std::wstring_view filePath, class ParticleEmitter* component);

public:
    void Initialize(HWND hwnd, UINT width, UINT height, FeatureLevel feature, bool isEditorMode);
    void UpdateAnimation(const float deltaTime);
    void Update(const float deltaTime);
    void Render();
    void Flip();
    void Finalize();

public:
    void             ResetSkyBox(std::string_view sceneName);
    void             OnResize(UINT width, UINT height);
    void XM_CALLCONV DebugDraw(std::string_view sceneName, const BoundingSphere& sphere, FXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV DebugDraw(std::string_view sceneName, const BoundingBox& box, FXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV DebugDraw(std::string_view sceneName, const BoundingOrientedBox& obb, FXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV DebugDraw(std::string_view sceneName, const BoundingFrustum& frustum, FXMVECTOR color = DirectX::Colors::White);  
    void XM_CALLCONV DebugDraw(std::string_view sceneName, FXMVECTOR origin, FXMVECTOR majorAxis, FXMVECTOR minorAxis, GXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV DebugDraw(std::string_view sceneName, FXMVECTOR origin, FXMVECTOR direction, bool normalize = true, FXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV DebugDraw(std::string_view sceneName, FXMVECTOR position, FXMVECTOR direction, float range, float innerCone, float outerCone, FXMVECTOR color = DirectX::Colors::White);

private:
    class Device*                   _device;
    class Renderer*                 _renderer;
    class CommandController*        _commandController;
    class DXResourceManager*        _dxResourceManager;
    class MultiRenderTargetManager* _multiRenderTargetManager;
    class ResourceManager*          _resourceManager;
    class ViewManager*              _viewManager;
    class AnimationCore*            _animationCore;
    class LightCore*                _lightCore;
    class ParticleManager*          _particleManager;
    class DebugDrawCore*            _debugDrawCore;
};