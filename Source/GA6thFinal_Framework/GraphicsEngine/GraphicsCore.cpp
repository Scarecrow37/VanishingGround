#include "pch.h"
#include "GraphicsCore.h"
#include "MeshRenderer.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"
#include "SDFTextRenderer.h"
#include "ParticleEmitter.h"
#include "Font.h"
#include "SDFFont.h"
#include "Animator.h"
#include "Light.h"
#include "MeshRenderer.h"
#include "SpriteRenderer.h"

namespace Global
{
    Device*                                        device;
    Renderer*                                      renderer;
    CommandController*                             commandController;
    DXResourceManager*                             dxResourceManager;
    MultiRenderTargetManager*                      multiRenderTargetManager;
    ResourceManager*                               resourceManager;
    ViewManager*                                   viewManager;
    AnimationCore*                                 animationCore;
    LightCore*                                     lightCore;
    ParticleManager*                               particleManager;
    DebugDrawCore*                                 debugDrawCore;
    RenderPassDatas*                               renderPassDatas;
    ModuleManager*                                 moduleManager;
    PipelineStateManager*                          pipelineStateManager;
    ThreadPool*                                    threadPool;
    SceneTransitionCore*                           sceneTransitionCore;

    bool                                           isRayTracing = false;
    std::unordered_map<std::wstring, std::wstring> shaderPathMappings;
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

const RenderPassImages& GraphicsCore::GetRenderPassImages() const
{
    return _renderPassDatas->GetRenderPassImages();
}

SceneTransitionCore* GraphicsCore::GetSceneTransitionCore() const 
{
    return _sceneTransitionCore;
}

const SIZE& GraphicsCore::GetResolution() const
{
    return _device->GetResolution();
}

void GraphicsCore::SetCamera(const std::string_view renderSceneName, std::shared_ptr<Camera> camera) const
{
    _renderer->SetCamera(renderSceneName, camera);
}

void GraphicsCore::SetEnvironmentSkyBox(const std::string_view renderSceneName, const std::wstring_view filePath) const
{
    _renderer->SetEnvironmentSkyBox(renderSceneName, filePath);
}

void GraphicsCore::SetIBLSkyBox(const std::string_view renderSceneName, const std::wstring_view filePath) const
{
    _renderer->SetIBLSkyBox(renderSceneName, filePath);
}

void GraphicsCore::SetCurrentScene(const std::string_view sceneName) const
{
    _renderer->SetCurrentScene(sceneName);
}

void GraphicsCore::SyncGlobalVariable()
{
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
    Global::sceneTransitionCore      = _sceneTransitionCore;
    Global::renderPassDatas          = _renderPassDatas;
    Global::dummyTextureHandle       = &_dummyTextureHandle;
}

void GraphicsCore::AddRenderScene(const std::string_view sceneName, const RenderTechniqueFlag flag) const
{
    _renderer->AddRenderScene(sceneName, flag);
}

void GraphicsCore::RegisterComponent(IAnimator* component) const
{
    _animationCore->RegisterAnimator(static_cast<Animator*>(component));
}

void GraphicsCore::RegisterComponent(const std::string_view renderSceneName, IMeshRenderer* component) const
{
    _renderer->RegisterRenderQueue(renderSceneName, static_cast<MeshRenderer*>(component));
}

void GraphicsCore::RegisterComponent(const std::string_view renderSceneName, ISpriteRenderer* component) const
{
    _renderer->RegisterRenderQueue(renderSceneName, static_cast<SpriteRenderer*>(component));
}

//void GraphicsCore::RegisterComponent(const std::string_view renderSceneName, TextRenderer* component) const
//{
//    _renderer->RegisterRenderQueue(renderSceneName, component);
//}

void GraphicsCore::RegisterComponent(const std::string_view renderSceneName, ILight* component) const
{
    _lightCore->RegisterLight(renderSceneName, static_cast<Light*>(component));
}

void GraphicsCore::RegisterComponent(std::string_view renderSceneName, ISDFTextRenderer* component) const
{
    _renderer->RegisterRenderQueue(renderSceneName, static_cast<SDFTextRenderer*>(component));
}

void GraphicsCore::CreateSDFTextRenderer(class ISDFTextRenderer** component) const
{
    SDFTextRenderer* textRenderer = new SDFTextRenderer;
    textRenderer->Initialize();
    textRenderer->AddReference();

    *component = textRenderer;
}

void GraphicsCore::CreateMeshRenderer(IMeshRenderer** component, const Matrix* worldMatrix) const
{
    MeshRenderer* meshRenderer = new MeshRenderer;
    meshRenderer->Initialize(worldMatrix);
    meshRenderer->AddReference();

    *component = meshRenderer;
}

void GraphicsCore::CreateSpriteRenderer(ISpriteRenderer** component, const Matrix* worldMatrix) const
{
    SpriteRenderer* spriteRenderer = new SpriteRenderer;
    spriteRenderer->Initialize(worldMatrix);
    spriteRenderer->AddReference();

    *component = spriteRenderer;
}

void GraphicsCore::CreateLight(ILight** component) const
{
    Light* light = new Light;
    light->AddReference();

    *component = light;
}

void GraphicsCore::SetResource(std::shared_ptr<Model> model, IMeshRenderer* component) const
{
    static_cast<MeshRenderer*>(component)->SetModel(model);
}

void GraphicsCore::LoadResource(std::wstring_view filePath, IMeshRenderer* component) const
{
    static_cast<MeshRenderer*>(component)->SetModel(_resourceManager->LoadResource<Model>(filePath));
}

void GraphicsCore::LoadResource(const std::wstring_view filePath, ISpriteRenderer* component) const
{
    static_cast<SpriteRenderer*>(component)->SetTexture(_resourceManager->LoadResource<Texture>(filePath));
}

//void GraphicsCore::LoadResource(const std::wstring_view filePath, TextRenderer* component) const
//{
//    component->SetFont(_resourceManager->LoadResource<Font>(filePath));
//}

void GraphicsCore::LoadResource(std::wstring_view filePath, ISDFTextRenderer* component) const
{
    static_cast<SDFTextRenderer*>(component)->SetFont(_resourceManager->LoadResource<SDFFont>(filePath));
}

void GraphicsCore::LoadTextureResource(std::wstring_view filePath, ParticleEmitter* component) const
{
    if (ParticleType::SPRITE == component->_particleType)
    {
        if (auto renderModule = component->_particleRenderModule->AsSprite())
        {
            renderModule->SetAlbedoTexture(_resourceManager->LoadResource<Texture>(filePath.data()));
        }
    }

    if (ParticleType::RIBBON == component->_particleType)
    {
        if (auto renderModule = component->_particleRenderModule->AsRibbon())
        {
            renderModule->SetAlbedoTexture(_resourceManager->LoadResource<Texture>(filePath.data()));
        }
    }
}

void GraphicsCore::LoadModelResource(const std::wstring_view filePath, ParticleEmitter* component) const
{
    if (component->_emitLocator->AsMeshSurfaceLocator())
    {
        _resourceManager->LoadResource<Model>(filePath);
    }
}

void GraphicsCore::Initialize(const HWND hwnd, const UINT width, const UINT height, const FeatureLevel feature, bool isEditorMode, bool isRayTracing)
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
    _pipelineStateManager     = new PipelineStateManager;
    _threadPool               = new ThreadPool;
    _sceneTransitionCore      = new SceneTransitionCore;

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
    Global::pipelineStateManager     = _pipelineStateManager;
    Global::threadPool               = _threadPool;
    Global::sceneTransitionCore      = _sceneTransitionCore;
    Global::isRayTracing             = isRayTracing;
    Global::dummyTextureHandle       = &_dummyTextureHandle;

    _device->SetUpDevice(hwnd, width, height, feature);
    _viewManager->Initialize();
    _device->Initialize();
    _device->ResetCommands();
    _renderer->Initialize();
    _moduleManager->Initialize();
    _threadPool->Initialize(5);
    _particleManager->Initialize(MAX_PARTICLE);

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
    //_animationCore->Update(deltaTime);
}

void GraphicsCore::Update(const float deltaTime)
{
    _threadPool->Update();    
    _resourceManager->Update();
    _animationCore->Update(deltaTime);
    _particleManager->Update(deltaTime);
    _lightCore->Update(deltaTime);
    _renderer->Update(deltaTime);
}

void GraphicsCore::Render() const
{
    _renderer->Render();
    _debugDrawCore->Render();
}

void GraphicsCore::Finalize() const
{
    _device->Finalize();

    auto SafeDelete = [](void* ptr)
    {
        if (ptr)
        {
            delete ptr;
            ptr = nullptr;
        }
    };

    SafeDelete(_threadPool);
    SafeDelete(_pipelineStateManager);
    SafeDelete(_moduleManager);
    SafeDelete(_renderPassDatas);
    SafeDelete(_debugDrawCore);
    SafeDelete(_commandController);
    SafeDelete(_dxResourceManager);
    SafeDelete(_particleManager);
    SafeDelete(_multiRenderTargetManager);
    SafeDelete(_resourceManager);
    SafeDelete(_viewManager);
    SafeDelete(_lightCore);
    SafeDelete(_animationCore);
    SafeDelete(_renderer);
    SafeDelete(_device);
    SafeDelete(_sceneTransitionCore);
}

void GraphicsCore::ClearGraphicsResource() const
{
    _renderer->ClearComponents();
    _renderer->ClearRenderQueue();
    _animationCore->ClearAnimationQueue();
    _lightCore->ClearLightQueue();
}

void GraphicsCore::Flip() const
{
    _renderer->Flip();
}

void GraphicsCore::ResetEnvironmentSkyBox(const std::string_view sceneName) const
{
    _renderer->ResetEnvironmentSkyBox(sceneName);
}

void GraphicsCore::ResetIBLSkyBox(const std::string_view sceneName) const
{
    _renderer->ResetIBLSkyBox(sceneName);
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

void GraphicsCore::DebugDraw2D(const std::string_view sceneName, FXMVECTOR origin, const float radius, FXMVECTOR color) const
{
    _debugDrawCore->DrawCircle(sceneName, origin, radius, color);
}