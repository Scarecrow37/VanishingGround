#include "pch.h"
#include "Renderer.h"

// Geometry
#include "Box.h"
#include "Cylinder.h"
#include "GeoSphere.h"
#include "Grid.h"
#include "Model.h"
#include "Quad.h"
#include "Sphere.h"

// Techniques
#include "BlendTechnique.h"
#include "BloomTechnique.h"
#include "EditorDrawTechnique.h"
#include "FontTechnique.h"
#include "PBRLitTechnique.h"
#include "ParticleRenderTechnique.h"
#include "RayTracingTechnique.h"
#include "SkyBoxRenderTechnique.h"
#include "UITechnique.h"

Renderer::Renderer()
{
    _isRaytracing = false;
}

Renderer::~Renderer() {}

D3D12_GPU_DESCRIPTOR_HANDLE Renderer::GetRenderSceneImage(std::string_view renderSceneName)
{
    auto iter = _renderScenes.find(renderSceneName.data());

    if (iter == _renderScenes.end())
    {
        std::wstring msg = L"Renderer::GetRenderSceneImage: RenderSceneName '" +
                           std::wstring(renderSceneName.begin(), renderSceneName.end()) + L"' is not registered.";
        GRAPHICS_ASSERT(false, msg.c_str());
    }

    auto& scene = iter->second;
    return scene->GetFinalImage();
}

std::shared_ptr<Camera> Renderer::GetCamera(std::string_view renderSceneName)
{
    auto iter = _renderScenes.find(renderSceneName.data());

    if (iter == _renderScenes.end())
    {
        std::wstring msg = L"Renderer::GetCamera: RenderSceneName '" +
                           std::wstring(renderSceneName.begin(), renderSceneName.end()) + L"' is not registered.";
        GRAPHICS_ASSERT(false, msg.c_str());
    }

    auto& scene = iter->second;
    return scene->GetCamera();
}

RenderScene* Renderer::GetRenderScene(std::string_view renderSceneName)
{
    auto iter = _renderScenes.find(renderSceneName.data());
    if (iter == _renderScenes.end())
    {
        std::wstring msg = L"Renderer::GetRenderScene: RenderSceneName '" +
                           std::wstring(renderSceneName.begin(), renderSceneName.end()) + L"' is not registered.";
        GRAPHICS_ASSERT(false, msg.c_str());
    }

    return iter->second.get();
}

void Renderer::SetCamera(std::string_view renderSceneName, std::shared_ptr<Camera> camera)
{
    auto iter = _renderScenes.find(renderSceneName.data());

    if (iter == _renderScenes.end())
    {
        std::wstring msg = L"Renderer::SetCamera: RenderSceneName '" +
                           std::wstring(renderSceneName.begin(), renderSceneName.end()) + L"' is not registered.";
        GRAPHICS_ASSERT(false, msg.c_str());
    }

    auto& scene = iter->second;
    scene->SetCamera(camera);
}

void Renderer::AddRenderScene(std::string_view sceneName, RenderTechniqueFlag flag)
{
    auto iter = _renderScenes.find(sceneName.data());
    if (iter != _renderScenes.end())
    {
        std::wstring msg = L"Renderer::AddRenderScene: RenderSceneName '" + std::wstring(sceneName.begin(), sceneName.end()) + L"' is already registered.";
        GRAPHICS_ASSERT(false, msg.c_str());
        return;
    }

    if (RenderTechniqueFlag::NONE == flag)
    {
        GRAPHICS_ASSERT(false, L"Renderer::AddRenderScene: No render techniques specified.");
        return;
    }

    std::unique_ptr<RenderScene> scene = std::make_unique<RenderScene>(sceneName);
    scene->InitializeRenderScene();

    // MeshRenderTarget Pass
    if (RenderTechniqueFlag::SKY_BOX_TECH & flag)
    {
        scene->AddRenderTechnique(std::make_unique<SkyBoxRenderTechnique>());
    }
    if (RenderTechniqueFlag::PBR_TECH & flag)
    {
        scene->AddRenderTechnique(std::make_unique<PBRLitTechnique>());
    }
    if (RenderTechniqueFlag::PARTICLE_TECH & flag)
    {
        scene->AddRenderTechnique(std::make_unique<ParticleRenderTechnique>());
    }
    if (RenderTechniqueFlag::EDITOR_DRAW_TECH & flag)
    {
        scene->AddRenderTechnique(std::make_unique<EditorDrawTechnique>());
    }

    // FinalRenderTarget Pass
    if (RenderTechniqueFlag::BLOOM_TECH & flag)
    {
        scene->AddRenderTechnique(std::make_unique<BloomTechnique>());
    }
    
    // Blend Pass
    scene->AddRenderTechnique(std::make_unique<BlendTechnique>());

    // UI Pass
    if (RenderTechniqueFlag::UI_TECH & flag)
    {
        scene->AddRenderTechnique(std::make_unique<UITechnique>());
    }
    if (RenderTechniqueFlag::FONT_TECH & flag)
    {
        scene->AddRenderTechnique(std::make_unique<FontTechnique>());
    }

    _renderScenes.try_emplace(sceneName.data(), std::move(scene));
}

void Renderer::RegisterRenderQueue(std::string_view sceneName, MeshRenderer* component)
{
    auto iter = _renderScenes.find(sceneName.data());

    if (iter == _renderScenes.end())
    {
        GRAPHICS_ASSERT(false, L"Renderer::RegisterRenderQueue : Render Scene Not Registered.");
    }

    auto& scene = iter->second;
    scene->RegisterOnRenderQueue(component);
}

void Renderer::RegisterRenderQueue(std::string_view sceneName, SpriteRenderer* component)
{
    auto iter = _renderScenes.find(sceneName.data());

    if (iter == _renderScenes.end())
    {
        GRAPHICS_ASSERT(false, L"Renderer::RegisterRenderQueue : Render Scene Not Registered.");
    }

    auto& scene = iter->second;
    scene->RegisterOnRenderQueue(component);
}

void Renderer::RegisterRenderQueue(std::string_view sceneName, FontRenderer* component)
{
    auto iter = _renderScenes.find(sceneName.data());

    if (iter == _renderScenes.end())
    {
        GRAPHICS_ASSERT(false, L"Renderer::RegisterRenderQueue : Render Scene Not Registered.");
    }

    auto& scene = iter->second;
    scene->RegisterOnRenderQueue(component);
}

void Renderer::SetSkyBox(std::string_view sceneName, std::wstring_view path)
{
    auto iter = _renderScenes.find(sceneName.data());

    if (iter == _renderScenes.end())
    {
        GRAPHICS_ASSERT(false, L"Renderer::RegisterRenderQueue : Render Scene Not Registered.");
    }

    auto& scene = iter->second;
    scene->SetSkyBox(path);
}

void Renderer::ResetSkyBox(std::string_view sceneName) 
{
    auto iter = _renderScenes.find(sceneName.data());

    if (iter == _renderScenes.end())
    {
        GRAPHICS_ASSERT(false, L"Renderer::RegisterRenderQueue : Render Scene Not Registered.");
    }

    auto& scene = iter->second;
    scene->ResetSkyBox();
}

void Renderer::Initialize()
{
    CreateDefaultResource();
}

void Renderer::Update()
{
    Global::device->ClearBackBuffer(D3D12_CLEAR_FLAG_DEPTH, { 0.5f, 0.5f, 0.5f, 1.f });

    for (auto& renderScene : _renderScenes)
    {
        renderScene.second->UpdateRenderScene();
    }
}

void Renderer::Render()
{
    ID3D12GraphicsCommandList* commandList = Global::device->GetCommandList();

    for (auto& renderScene : _renderScenes)
    {
        renderScene.second->Execute(commandList);
    }
    
    Global::device->SetBackBuffer();
}

void Renderer::Flip()
{    
    Global::device->Execute();
    Global::device->Flip();
    Global::device->ResetCommands();
    Global::device->ResetComputeCommands();
}

void Renderer::CreateDefaultResource()
{
    CreateDefaultGeometry();
    CreateDefaultTexture();
    CreateDefaultRenderTarget();
}

void Renderer::CreateDefaultGeometry()
{
    std::unique_ptr<Box> box = std::make_unique<Box>();
    box->Initialize(1.f, 1.f, 1.f);

    std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();
    sphere->Initialize(1.f, 20, 20);

    std::unique_ptr<GeoSphere> geoSphere = std::make_unique<GeoSphere>();
    geoSphere->Initialize(1.f, 5);

    std::unique_ptr<Cylinder> cylinder = std::make_unique<Cylinder>();
    cylinder->Initialize(0.5f, 0.3f, 2.f, 20, 20);

    std::unique_ptr<Grid> grid = std::make_unique<Grid>();
    grid->Initialize(20.f, 30.f, 4, 4);

    std::unique_ptr<Quad> quad = std::make_unique<Quad>();
    quad->Initialize(-1.0f, 1.0f, 2.0f, 2.0f, 0.0f);

    std::unique_ptr<Quad> halfQuad = std::make_unique<Quad>();
    halfQuad->Initialize(-0.5f, 0.5f, 1.0f, 1.0f, 0.0f);

    std::shared_ptr<Model>    geometry;
    std::unique_ptr<BaseMesh> baseMesh;

    auto& resourceManager = Global::resourceManager;

    baseMesh = std::move(box);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager->AddResource(L"Box", geometry);

    baseMesh = std::move(cylinder);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager->AddResource(L"Cylinder", geometry);

    baseMesh = std::move(sphere);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager->AddResource(L"Sphere", geometry);

    baseMesh = std::move(geoSphere);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager->AddResource(L"GeoSphere", geometry);

    baseMesh = std::move(grid);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager->AddResource(L"Grid", geometry);

    baseMesh = std::move(quad);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager->AddResource(L"Quad", geometry);

    baseMesh = std::move(halfQuad);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager->AddResource(L"HalfQuad", geometry);
}

void Renderer::CreateDefaultTexture()
{
    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension           = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width               = 1;
    texDesc.Height              = 1;
    texDesc.DepthOrArraySize    = 1;
    texDesc.MipLevels           = 1;
    texDesc.Format              = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count    = 1;
    texDesc.Layout              = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags               = D3D12_RESOURCE_FLAG_NONE;

    ID3D12Device*          device = Global::device->GetDevice();
    ComPtr<ID3D12Resource> texture;

    auto defaultHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    device->CreateCommittedResource(&defaultHeapProp, D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_COPY_DEST,
                                    nullptr, IID_PPV_ARGS(&texture));

    UINT64                 uploadBufferSize;
    ComPtr<ID3D12Resource> uploadHeap;

    device->GetCopyableFootprints(&texDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);
    auto uploadHeapProp       = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto uploadBufferSizeProp = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
    device->CreateCommittedResource(&uploadHeapProp, D3D12_HEAP_FLAG_NONE, &uploadBufferSizeProp,
                                    D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                    IID_PPV_ARGS(&uploadHeap));

    D3D12_SUBRESOURCE_DATA textureData   = {};
    static const uint8_t   blackPixel[4] = {0, 0, 0, 255};
    textureData.pData                    = blackPixel;
    textureData.RowPitch                 = 4;
    textureData.SlicePitch               = 4;

    ID3D12GraphicsCommandList* commandList = Global::device->GetCommandList();
    UpdateSubresources(commandList, texture.Get(), uploadHeap.Get(), 0, 0, 1, &textureData);
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    commandList->ResourceBarrier(1, &barrier);

    std::shared_ptr<Texture> textureResource = std::make_shared<Texture>();
    textureResource->SetResource(texture.Get());
    textureResource->CreateShaderResourceView();

    Global::resourceManager->AddResource("BlackTexture", textureResource);
    _defaultResource.push_back(textureResource);

    Global::device->UploadResource(uploadHeap);
}

void Renderer::CreateDefaultRenderTarget()
{
    std::initializer_list<std::string_view> defaultRenderTargets = {"1024x1024", "512x512", "256x256", "128x128", "64x64", "32x32", "16x16", "8x8", "4x4", "2x2", "1x1"};
    SharedResource<RenderTarget> renderTarget;
    auto&                        multiRenderTargetManager = Global::multiRenderTargetManager;
    DXGI_MODE_DESC               mode{.Width = 1024, .Height = 1024, .Format = DXGI_FORMAT_R32G32B32A32_FLOAT};

    for (auto& defaultRenderTarget : defaultRenderTargets)
    {
        renderTarget = MakeSharedResource<RenderTarget>();
        renderTarget->Initialize(mode, 0.f);

        mode.Width >>= 1;
        mode.Height >>= 1;

        multiRenderTargetManager->AddRenderTarget(defaultRenderTarget, renderTarget);
        multiRenderTargetManager->AddRenderTargetGroup("Mipmap", defaultRenderTarget.data());
    }

    mode = Global::device->GetMode();
    mode.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    multiRenderTargetManager->InitializeRenderTargetPool(4, mode);
}