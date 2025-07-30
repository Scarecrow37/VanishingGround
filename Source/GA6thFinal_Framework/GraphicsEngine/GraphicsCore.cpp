#include "pch.h"
#include "GraphicsCore.h"
#include "MeshRenderer.h"
#include "SpriteRenderer.h"
#include "FontRenderer.h"
#include "ParticleEmitter.h"
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
    RenderPassDatas*          renderPassDatas;
    ModuleManager*            moduleManager;
};

ParticleManager* GraphicsCore::GetParticleManager() const
{
    return _particleManager;
}

ResourceManager* GraphicsCore::GetResourceManager() const
{
    return _resourceManager;
}

std::shared_ptr<Camera> GraphicsCore::GetCamera(const std::string_view cameraName) const
{
    return _renderer->GetCamera(cameraName);
}

D3D12_GPU_DESCRIPTOR_HANDLE GraphicsCore::GetRenderSceneImage(const std::string_view renderSceneName) const
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

RenderPassProperties& GraphicsCore::GetRenderPassProperties() const
{
    return _renderPassDatas->GetRenderPassProperties();
}

void GraphicsCore::SetCamera(const std::string_view renderSceneName, std::shared_ptr<Camera> camera) const
{
    _renderer->SetCamera(renderSceneName, camera);
}

void GraphicsCore::SetSkyBox(const std::string_view renderSceneName, const std::wstring_view filePath) const
{
    _renderer->SetSkyBox(renderSceneName, filePath);
}

void GraphicsCore::SetCurrentScene(const std::string_view sceneName) const
{
    _renderer->SetCurrentScene(sceneName);
}

void GraphicsCore::AddRenderScene(const std::string_view sceneName, const RenderTechniqueFlag flag) const
{
    _renderer->AddRenderScene(sceneName, flag);
}

void GraphicsCore::RegisterComponent(Animator* component) const
{
    _animationCore->RegisterAnimator(component);
}

void GraphicsCore::RegisterComponent(const std::string_view renderSceneName, MeshRenderer* component) const
{
    _renderer->RegisterRenderQueue(renderSceneName, component);
}

void GraphicsCore::RegisterComponent(const std::string_view renderSceneName, SpriteRenderer* component) const
{
    _renderer->RegisterRenderQueue(renderSceneName, component);
}

void GraphicsCore::RegisterComponent(const std::string_view renderSceneName, FontRenderer* component) const
{
    _renderer->RegisterRenderQueue(renderSceneName, component);
}

void GraphicsCore::RegisterComponent(const std::string_view renderSceneName, Light* component) const
{
    _lightCore->RegisterLight(renderSceneName, component);
}

void GraphicsCore::LoadResource(const std::wstring_view filePath, MeshRenderer* component) const
{    
    component->SetModel(_resourceManager->LoadResource<Model>(filePath));    
}

void GraphicsCore::LoadResource(const std::wstring_view filePath, SpriteRenderer* component) const
{
    component->SetTexture(_resourceManager->LoadResource<Texture>(filePath));
}

void GraphicsCore::LoadResource(const std::wstring_view filePath, FontRenderer* component) const
{
    component->SetFont(_resourceManager->LoadResource<Font>(filePath));
}

void GraphicsCore::LoadTextureResource(std::wstring_view filePath, ParticleEmitter* component) const
{
    if (ParticleType::SPRITE == component->_particleType)
    {
        static_cast<SpriteModule*>(component->_particleRenderModule)
            ->SetAlbedoTexture(_resourceManager->LoadResource<Texture>(filePath.data()));
    }
    if (ParticleType::RIBBON == component->_particleType)
    {
        static_cast<RibbonModule*>(component->_particleRenderModule)
            ->SetAlbedoTexture(_resourceManager->LoadResource<Texture>(filePath.data()));
    }
}
void GraphicsCore::LoadModelResource(std::wstring_view filePath, ParticleEmitter* component) const
{
    static_cast<MeshSurfaceLocator*>(component->_emitLocator)->SetModelPath(filePath.data());
    static_cast<MeshSurfaceLocator*>(component->_emitLocator)->LoadVerticesFromModel(_resourceManager->LoadResource<Model>(filePath.data()));
}

void GraphicsCore::Initialize(const HWND hwnd, const UINT width, const UINT height, const FeatureLevel feature, bool isEditorMode)
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
    _renderPassDatas          = new RenderPassDatas;
    _moduleManager            = new ModuleManager;

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
    Global::renderPassDatas          = _renderPassDatas;
    Global::moduleManager            = _moduleManager;

    _device->SetUpDevice(hwnd, width, height, feature);
    _viewManager->Initialize();
    _device->Initialize();
    _device->ResetCommands();
    _particleManager->Initialize(MAX_PARTICLE);
    _renderer->Initialize();
    _moduleManager->Initialize();

    auto commandList = _device->GetCommandList();
    commandList->Close();

    _commandController->ExecuteCommand(CommandQueueType::GRAPHICS_QUEUE, commandList);
    _device->GPUSync();

    _device->ResetCommands();
    _device->ResetComputeCommands();

    _debugDrawCore->Initialize();
}

void GraphicsCore::UpdateAnimation(const float deltaTime) const
{
    _animationCore->Update(deltaTime);
}

void GraphicsCore::Update(const float deltaTime) const
{
    _particleManager->Update(deltaTime);
    _lightCore->Update(deltaTime);
    _renderer->Update();
}

void GraphicsCore::Render() const
{
    _renderer->Render();
    _debugDrawCore->Render();
}

void GraphicsCore::Finalize() const
{
    _device->Finalize();

    delete _moduleManager;
    delete _renderPassDatas;
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

void GraphicsCore::Flip() const
{
    _renderer->Flip();
}

void GraphicsCore::ResetSkyBox(const std::string_view sceneName) const
{
    _renderer->ResetSkyBox(sceneName);
}

void GraphicsCore::OnResize(const UINT width, const UINT height) const
{
    _device->OnResize(width, height);
}

void XM_CALLCONV GraphicsCore::DebugDraw3D(const std::string_view sceneName, const BoundingSphere& sphere, FXMVECTOR color) const
{
    _debugDrawCore->Draw(sceneName, sphere, color);
}

void XM_CALLCONV GraphicsCore::DebugDraw3D(const std::string_view sceneName, const BoundingBox& box, FXMVECTOR color) const
{
    _debugDrawCore->Draw(sceneName, box, color);
}

void XM_CALLCONV GraphicsCore::DebugDraw3D(const std::string_view sceneName, const BoundingOrientedBox& obb, FXMVECTOR color) const
{
    _debugDrawCore->Draw(sceneName, obb, color);
}

void XM_CALLCONV GraphicsCore::DebugDraw3D(const std::string_view sceneName, const BoundingFrustum& frustum, FXMVECTOR color) const
{
    _debugDrawCore->Draw(sceneName, frustum, color);
}

void XM_CALLCONV GraphicsCore::DebugDraw3D(const std::string_view sceneName, FXMVECTOR origin, FXMVECTOR majorAxis,
                                           FXMVECTOR minorAxis, GXMVECTOR color) const
{
    _debugDrawCore->DrawRing(sceneName, origin, majorAxis, minorAxis, color);
}

void XM_CALLCONV GraphicsCore::DebugDraw3D(const std::string_view sceneName, FXMVECTOR origin, FXMVECTOR direction,
                                           const bool normalize, FXMVECTOR color) const
{
    _debugDrawCore->DrawRay(sceneName, origin, direction, normalize, color);
}

void XM_CALLCONV GraphicsCore::DebugDraw3D(const std::string_view sceneName, FXMVECTOR position, FXMVECTOR direction,
                                           const float range, const float innerCone, const float outerCone, FXMVECTOR color) const
{
    _debugDrawCore->DrawSpotLight(sceneName, position, direction, range, innerCone, outerCone, color);
}

void XM_CALLCONV GraphicsCore::DebugDraw2D(const std::string_view sceneName, FXMVECTOR pointA, FXMVECTOR pointB,
                                           FXMVECTOR pointC, GXMVECTOR pointD, HXMVECTOR color) const
{
    _debugDrawCore->DrawQuad(sceneName, pointA, pointB, pointC, pointD, color);
}

void XM_CALLCONV GraphicsCore::DebugDraw2D(const std::string_view sceneName, FXMVECTOR pointA, FXMVECTOR pointB,
                                           FXMVECTOR color) const
{
    _debugDrawCore->DrawLine(sceneName, pointA, pointB, color);
}