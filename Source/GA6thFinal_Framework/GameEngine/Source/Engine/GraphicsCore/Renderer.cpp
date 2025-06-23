#include "pch.h"
#include "Renderer.h"
#include "RenderTarget.h"
#include "Box.h"
#include "Cylinder.h"
#include "GeoSphere.h"
#include "Grid.h"
#include "Model.h"
#include "PBRLitTechnique.h"
#include "Quad.h"
#include "RenderScene.h"
#include "RendererFileEvent.h"
#include "SkyBoxRenderTechnique.h"
#include "BloomTechnique.h"
#include "ToneMappingTechnique.h"
#include "Sphere.h"

Renderer::Renderer()
    : _currnetState(0)
{
}

Renderer::~Renderer() {}

D3D12_GPU_DESCRIPTOR_HANDLE Renderer::GetRenderSceneImage(std::string_view renderSceneName)
{
    auto iter = _renderScenes.find(std::string(renderSceneName));

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
    auto iter = _renderScenes.find(std::string(renderSceneName));

    if (iter == _renderScenes.end())
    {
        std::wstring msg = L"Renderer::GetRenderSceneImage: RenderSceneName '" +
                           std::wstring(renderSceneName.begin(), renderSceneName.end()) + L"' is not registered.";
        GRAPHICS_ASSERT(false, msg.c_str());
    }

    auto& scene = iter->second;
    return scene->GetCamera();
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

void Renderer::SetSkyBox(std::string_view sceneName, std::string_view path) 
{
    auto iter = _renderScenes.find(sceneName.data());

    if (iter == _renderScenes.end())
    {
        GRAPHICS_ASSERT(false, L"Renderer::RegisterRenderQueue : Render Scene Not Registered.");
    }

    auto& scene = iter->second;
    scene->SetSkyBox(path);
}

void Renderer::SetSkyBox(std::string_view path)
{
    // 얼추 게임 씬 나오면 그거 바꿔야할텐데.
    auto  iter  = _renderScenes.find("Editor");
    auto& scene = iter->second;
    scene->SetSkyBox(path);
}

void Renderer::ResetSkyBox()
{
    auto iter = _renderScenes.find("Editor");
    auto& scene = iter->second;
    scene->ResetSkyBox();
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

    std::unique_ptr<RenderScene> editorScene = std::make_unique<RenderScene>("Editor");
    editorScene->InitializeRenderScene();
    editorScene->AddRenderTechnique(std::make_unique<SkyBoxRenderTechnique>());
    editorScene->AddRenderTechnique(std::make_unique<PBRLitTechnique>());
    editorScene->AddRenderTechnique(std::make_unique<BloomTechnique>());
    //editorScene->AddRenderTechnique(std::make_unique<ToneMappingTechnique>());

    _renderScenes["Editor"] = std::move(editorScene);

    if constexpr (IS_EDITOR)
    {
        // Model Viewer Scene
        std::unique_ptr<RenderScene> modelViewerScene = std::make_unique<RenderScene>("ModelViewer");
        modelViewerScene->InitializeRenderScene();
        modelViewerScene->AddRenderTechnique(std::make_unique<PBRLitTechnique>());
        _renderScenes["ModelViewer"] = std::move(modelViewerScene);        

        // Renderer File Event
        _rendererFileEvent = std::make_unique<RendererFileEvent>();
        UmFileSystem.RegisterFileEventSubscriber(_rendererFileEvent.get(), {".png", ".dds", ".fbx", ".UmModel"});
    }
}

void Renderer::Update()
{
    UmDevice.ClearBackBuffer(D3D12_CLEAR_FLAG_DEPTH, { 0.5f, 0.5f, 0.5f, 1.f });

    for (auto& renderScene : _renderScenes)
    {
        renderScene.second->UpdateRenderScene();
    }
}

void Renderer::Render()
{
    ID3D12GraphicsCommandList* commandList = UmDevice.GetCommandList();

    for (auto& renderScene : _renderScenes)
    {
        renderScene.second->Execute(commandList);
    }
    if constexpr (IS_EDITOR)
        UmDevice.SetBackBuffer();
}

void Renderer::Flip()
{
    auto& device = UmDevice;

    device.Execute();
    device.Flip();
    device.ResetCommands();
    device.ResetComputeCommands();
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
    sphere                         = std::make_unique<Sphere>();
    sphere->Initialize(1.f, 20, 20);

    std::unique_ptr<GeoSphere> geoSphere = std::make_unique<GeoSphere>();
    geoSphere                            = std::make_unique<GeoSphere>();
    geoSphere->Initialize(1.f, 5);

    std::unique_ptr<Cylinder> cylinder = std::make_unique<Cylinder>();
    cylinder                           = std::make_unique<Cylinder>();
    cylinder->Initialize(0.5f, 0.3f, 2.f, 20, 20);

    std::unique_ptr<Grid> grid = std::make_unique<Grid>();
    grid                       = std::make_unique<Grid>();
    grid->Initialize(20.f, 30.f, 4, 4);

    std::unique_ptr<Quad> quad = std::make_unique<Quad>();
    quad                       = std::make_unique<Quad>();
    quad->Initialize(-1.0f, 1.0f, 2.0f, 2.0f, 0.0f);

    std::shared_ptr<Model>    geometry;
    std::unique_ptr<BaseMesh> baseMesh;

    auto& resourceManager = UmResourceManager;

    baseMesh = std::move(box);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager.AddResource(L"Box", geometry);

    baseMesh = std::move(cylinder);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager.AddResource(L"Cylinder", geometry);

    baseMesh = std::move(sphere);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager.AddResource(L"Sphere", geometry);

    baseMesh = std::move(geoSphere);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager.AddResource(L"GeoSphere", geometry);

    baseMesh = std::move(grid);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager.AddResource(L"Grid", geometry);

    baseMesh = std::move(quad);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager.AddResource(L"Quad", geometry);
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

    ID3D12Device*          device = UmDevice.GetDevice();
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

    ID3D12GraphicsCommandList* commandList = UmDevice.GetCommandList();
    UpdateSubresources(commandList, texture.Get(), uploadHeap.Get(), 0, 0, 1, &textureData);
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    commandList->ResourceBarrier(1, &barrier);

    std::shared_ptr<Texture> textureResource = std::make_shared<Texture>();
    textureResource->SetResource(texture.Get());
    textureResource->CreateShaderResourceView();

    UmResourceManager.AddResource("BlackTexture", textureResource);
    _defaultResource.push_back(textureResource);

    UmDevice.UploadResource(uploadHeap);
}

void Renderer::CreateDefaultRenderTarget()
{
    std::unique_ptr<RenderTarget> renderTarget;
    std::initializer_list<std::string_view> defaultRenderTargets = {"1024x1024", "512x512", "256x256", "128x128", "64x64", "32x32", "16x16", "8x8", "4x4", "2x2", "1x1"};
    UINT size[]{1024, 1024};

    auto& multiRenderTargetManager = UmMultiRenderTargetManager;
    for (auto& defaultRenderTarget : defaultRenderTargets)
    {
        renderTarget = std::make_unique<RenderTarget>();
        renderTarget->Initialize(size[0], size[1], DXGI_FORMAT_R32G32B32A32_FLOAT, 0.f);
        renderTarget->CreateShaderResourceView();
        size[0] >>= 1;
        size[1] >>= 1;

        multiRenderTargetManager.AddRenderTarget(defaultRenderTarget, std::move(renderTarget));
        multiRenderTargetManager.AddRenderTargetGroup("Mipmap", defaultRenderTarget.data());
    }

    auto mode = UmDevice.GetMode();
    mode.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    multiRenderTargetManager.InitializeRenderTargetPool(4, mode);
}

void Renderer::InitializeImgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // IF using Docking Branch
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular
    // ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding              = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImFontConfig fontConfig{};
    ImFont*      mainFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 20.0f, &fontConfig,
                                                         io.Fonts->GetGlyphRangesKorean());

    std::string fontFileName = "Font Awesome 6 Free-Regular-400.ttf";
    File::Path  fontPath     = UmFileSystem.GetRootPath();
    fontPath /= fontFileName;
    if (true == std::filesystem::exists(fontPath.generic_string()))
    {
        const ImWchar icons_ranges[] = {0xf000, 0xf3ff, 0}; // FontAwesome 유니코드 범위
        ImFontConfig  config;

        config.MergeMode  = true;
        config.PixelSnapH = true;

        ImFontAtlas* atlas    = io.Fonts;
        ImFont*      iconFont = atlas->AddFontFromFileTTF(fontPath.string().c_str(), 15.0f, &config, icons_ranges);
    }
    io.Fonts->Build();

    auto descriptorHeap = UmViewManager.GetShaderResourceHeap();
    auto cpuHandle      = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    auto gpuHandle      = descriptorHeap->GetGPUDescriptorHandleForHeapStart();

    ImGui_ImplWin32_Init(Global::engineCore->App.GetHwnd());
    ImGui_ImplDX12_Init(UmDevice.GetDevice(), static_cast<int>(SWAPCHAIN_BUFFER_COUNT), UmDevice.GetBackBufferFormat(), descriptorHeap, cpuHandle, gpuHandle);
}

void Renderer::PreUnInitializeImgui()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Renderer::ImguiBegin()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

void Renderer::ImguiEnd()
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Render();

    ID3D12DescriptorHeap* descriptorHeaps[] = {UmViewManager.GetShaderResourceHeap()};
    UmDevice.GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), UmDevice.GetCommandList());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault(nullptr, nullptr);
    }
}