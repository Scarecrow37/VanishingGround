#include "pch.h"
#include "GraphicsCore.h"
#include "MeshRenderer.h"
#include "SpriteRenderer.h"
#include "FontRenderer.h"
#include "Font.h"

namespace Global
{
    Device*                   device;
    Renderer*                 renderer;
    CommandController*        commandController;
    DXResourceManager*        dxResourceManager;
    MultiRenderTargetManager* multiRenderTargetManager;
    ResourceManager*          resourceManager;
    ViewManager*              viewManager;
    AnimationCore*            animationCore;
    LightCore*                lightCore;
    ParticleManager*          particleManager;
    DebugDrawCore*            debugDrawCore;
}

ParticleManager* GraphicsCore::GetParticleManager() const
{
    return _particleManager;
}

ResourceManager* GraphicsCore::GetResourceManager() const
{
    return _resourceManager;
}

std::shared_ptr<Camera> GraphicsCore::GetCamera(std::string_view cameraName) const
{
    return _renderer->GetCamera(cameraName);
}

D3D12_GPU_DESCRIPTOR_HANDLE GraphicsCore::GetRenderSceneImage(std::string_view renderSceneName) const
{
    return _renderer->GetRenderSceneImage(renderSceneName);
}

ID3D12DescriptorHeap* GraphicsCore::GetShaderResourceDescriptorHeap() const
{
    return _viewManager->GetShaderResourceHeap();
}

ID3D12Device* GraphicsCore::GetDevice() const
{
    return _device->GetDevice();
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsCore::GetBackBufferHandle() const
{
    return _device->GetBackBufferHandle();
}

ID3D12GraphicsCommandList* GraphicsCore::GetCommandList() const
{
    return _device->GetCommandList();
}

void GraphicsCore::SetCamera(std::string_view renderSceneName, std::shared_ptr<Camera> camera)
{
    _renderer->SetCamera(renderSceneName, camera);
}

void GraphicsCore::SetSkyBox(std::string_view renderSceneName, std::wstring_view filePath)
{
    _renderer->SetSkyBox(renderSceneName, filePath);
}

void GraphicsCore::SetCurrentScene(std::string_view sceneName)
{
    _renderer->SetCurrentScene(sceneName);
}

void GraphicsCore::AddRenderScene(std::string_view sceneName, RenderTechniqueFlag flag)
{
    _renderer->AddRenderScene(sceneName, flag);
}

void GraphicsCore::RegisterComponent(Animator* component)
{
    _animationCore->RegisterAnimator(component);
}

void GraphicsCore::RegisterComponent(std::string_view renderSceneName, MeshRenderer* component)
{
    _renderer->RegisterRenderQueue(renderSceneName, component);
}

void GraphicsCore::RegisterComponent(std::string_view renderSceneName, SpriteRenderer* component)
{
    _renderer->RegisterRenderQueue(renderSceneName, component);
}

void GraphicsCore::RegisterComponent(std::string_view renderSceneName, FontRenderer* component)
{
    _renderer->RegisterRenderQueue(renderSceneName, component);
}

void GraphicsCore::RegisterComponent(std::string_view renderSceneName, Light* component)
{
    _lightCore->RegisterLight(renderSceneName, component);
}

void GraphicsCore::LoadResource(std::wstring_view filePath, MeshRenderer* component)
{    
    component->SetModel(_resourceManager->LoadResource<Model>(filePath));    
}

void GraphicsCore::LoadResource(std::wstring_view filePath, SpriteRenderer* component)
{
    component->SetTexture(_resourceManager->LoadResource<Texture>(filePath));
}

void GraphicsCore::LoadResource(std::wstring_view filePath, FontRenderer* component)
{
    component->SetFont(_resourceManager->LoadResource<Font>(filePath));
}

void GraphicsCore::Initialize(HWND hwnd, UINT width, UINT height, FeatureLevel feature, bool isEditorMode)
{
    _device                   = new Device;
    _renderer                 = new Renderer;
    _animationCore            = new AnimationCore;
    _lightCore                = new LightCore;
    _viewManager              = new ViewManager;
    _resourceManager          = new ResourceManager;
    _multiRenderTargetManager = new MultiRenderTargetManager;
    _particleManager          = new ParticleManager;
    _dxResourceManager        = new DXResourceManager;
    _commandController        = new CommandController;
    _debugDrawCore            = new DebugDrawCore;

    Global::device                   = _device;
    Global::renderer                 = _renderer;
    Global::animationCore            = _animationCore;
    Global::lightCore                = _lightCore;
    Global::viewManager              = _viewManager;
    Global::resourceManager          = _resourceManager;
    Global::multiRenderTargetManager = _multiRenderTargetManager;
    Global::particleManager          = _particleManager;
    Global::dxResourceManager        = _dxResourceManager;
    Global::commandController        = _commandController;
    Global::debugDrawCore            = _debugDrawCore;

    _device->SetUpDevice(hwnd, width, height, feature);
    _viewManager->Initialize();
    _device->Initialize();
    _device->ResetCommands();
    _particleManager->Initialize(MAX_PARTICLE);
    _renderer->Initialize();

    auto commandList = _device->GetCommandList();
    commandList->Close();

    _commandController->ExecuteCommand(CommandQueueType::GRAPHICS_QUEUE, commandList);
    _device->GPUSync();

    _device->ResetCommands();
    _device->ResetComputeCommands();

    _debugDrawCore->Initialize();
}

void GraphicsCore::UpdateAnimation(const float deltaTime)
{
    _animationCore->Update(deltaTime);
}

void GraphicsCore::Update(const float deltaTime)
{
    _particleManager->Update(deltaTime);
    _lightCore->Update(deltaTime);
    _renderer->Update();
}

void GraphicsCore::Render()
{
    _renderer->Render();
    _debugDrawCore->Render();
}

void GraphicsCore::Finalize()
{
    _device->Finalize();

    delete _debugDrawCore;
    delete _commandController;
    delete _dxResourceManager;
    delete _particleManager;
    delete _multiRenderTargetManager;
    delete _resourceManager;
    delete _viewManager;
    delete _lightCore;
    delete _animationCore;
    delete _renderer;
    delete _device;
}

void GraphicsCore::Flip()
{
    _renderer->Flip();
}

void GraphicsCore::ResetSkyBox(std::string_view sceneName)
{
    _renderer->ResetSkyBox(sceneName);
}

void GraphicsCore::OnResize(UINT width, UINT height)
{
    _device->OnResize(width, height);
}

void XM_CALLCONV GraphicsCore::DebugDraw(std::string_view sceneName, const BoundingSphere& sphere, FXMVECTOR color)
{
    _debugDrawCore->Draw(sceneName, sphere, color);
}

void XM_CALLCONV GraphicsCore::DebugDraw(std::string_view sceneName, const BoundingBox& box, FXMVECTOR color)
{
    _debugDrawCore->Draw(sceneName, box, color);
}

void XM_CALLCONV GraphicsCore::DebugDraw(std::string_view sceneName, const BoundingOrientedBox& obb, FXMVECTOR color)
{
    _debugDrawCore->Draw(sceneName, obb, color);
}

void XM_CALLCONV GraphicsCore::DebugDraw(std::string_view sceneName, const BoundingFrustum& frustum, FXMVECTOR color)
{
    _debugDrawCore->Draw(sceneName, frustum, color);
}

void XM_CALLCONV GraphicsCore::DebugDraw(std::string_view sceneName, FXMVECTOR origin, FXMVECTOR majorAxis, FXMVECTOR minorAxis, GXMVECTOR color)
{
    _debugDrawCore->DrawRing(sceneName, origin, majorAxis, minorAxis, color);
}

void XM_CALLCONV GraphicsCore::DebugDraw(std::string_view sceneName, FXMVECTOR origin, FXMVECTOR direction, bool normalize, FXMVECTOR color)
{
    _debugDrawCore->DrawRay(sceneName, origin, direction, normalize, color);
}

void XM_CALLCONV GraphicsCore::DebugDraw(std::string_view sceneName, FXMVECTOR position, FXMVECTOR direction, float range, float innerCone, float outerCone, FXMVECTOR color)
{
    _debugDrawCore->DrawSpotLight(sceneName, position, direction, range, innerCone, outerCone, color);
}