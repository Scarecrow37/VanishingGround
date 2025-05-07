#include "pch.h"
#include "Renderer.h"
#include "UmScripts.h"

// Editor
#include "NonPBRLitTechnique.h"
#include "RendererFileEvent.h"

#define SeongU01
#ifdef SeongU01
#include "Box.h"
#include "Sphere.h"
#include "GeoSphere.h"
#include "Cylinder.h"
#include "Grid.h"
#include "Quad.h"
#include "RenderScene.h"
#include "PBRLitTechnique.h"
#endif

Renderer::Renderer()
    : _currnetState(0)
    , _currentImGuiImageIndex(1)

{
}

Renderer::~Renderer()
{
}

void Renderer::RegisterRenderQueue(bool** isActive, MeshRenderer* component, std::string_view sceneName)
{
    auto iter = _renderScenes.find(sceneName.data());

    if (iter == _renderScenes.end())
    {
        ASSERT(false, L"Renderer::RegisterRenderQueue : Render Scene Not Registered.");
    }
    auto scene = iter->second;
    scene->RegisterOnRenderQueue(isActive,component);
}

void Renderer::Initialize()
{
    /*if (IS_EDITOR)
    {
    }
    else
    {
    }*/
    //std::shared_ptr<RenderScene> testRenderScene = std::make_shared<RenderScene>();
    //testRenderScene->InitializeRenderScene(1);
    //std::shared_ptr<PBRTechnique> pbrTech = std::make_shared<PBRTechnique>();
    //testRenderScene->AddRenderTechnique("PBR", pbrTech);

    //_renderScenes["TEST PBR"] = testRenderScene;
    std::shared_ptr<RenderScene> editorScene = std::make_shared<RenderScene>();
    editorScene->InitializeRenderScene();
    std::shared_ptr<PBRLitTechnique> pbrTech = std::make_shared<PBRLitTechnique>();
    editorScene->AddRenderTechnique(pbrTech);
    _renderScenes["Editor"] = editorScene;

    if constexpr (IS_EDITOR)
    {
        // Model Viewer Scene
        std::shared_ptr<RenderScene> modelViewerScene = std::make_shared<RenderScene>();
        modelViewerScene->InitializeRenderScene();
        modelViewerScene->AddRenderTechnique(std::make_shared<PBRLitTechnique>());
        _renderScenes["ModelViewer"] = modelViewerScene;

        // Renderer File Event
        _rendererFileEvent = std::make_unique<RendererFileEvent>();
        UmFileSystem.RegisterFileEventNotifier(_rendererFileEvent.get(), {".png", ".dds", ".fbx"});
    }
}

void Renderer::Update()
{
    //UmMainCamera.Update();

	UmDevice.ResetCommands();
	//UpdateFrameResource();
	UmDevice.ClearBackBuffer(D3D12_CLEAR_FLAG_DEPTH, { 0.5f, 0.5f, 0.5f, 1.f });

    for (auto& renderScene : _renderScenes)
    {
        renderScene.second->UpdateRenderScene();
    }
}

void Renderer::Render()
{
	ComPtr<ID3D12GraphicsCommandList> commandList = UmDevice.GetCommandList();

    for (auto& renderScene : _renderScenes)
    {
        renderScene.second->Execute(commandList.Get());
    }
    if constexpr(IS_EDITOR) UmDevice.SetBackBuffer();
}

void Renderer::Flip()
{
    UmDevice.Flip();
    // 임시 ImGUI Image Index 찾는 구조 나중에 수정
    // ImGUI Descriptor Index 초기화 (0 은 ImGUI Font)
    _currentImGuiImageIndex = 1;
}

D3D12_GPU_DESCRIPTOR_HANDLE Renderer::GetRenderSceneImage(std::string_view renderSceneName)
{
    auto iter = _renderScenes.find(std::string(renderSceneName));
    if (iter != _renderScenes.end())
    {
        auto scene = iter->second;
        return SceneView(scene.get());
    }
    else
    {
        std::wstring msg = L"Renderer::GetRenderSceneImage: RenderSceneName '" +
                           std::wstring(renderSceneName.begin(), renderSceneName.end()) + L"' is not registered.";
        ASSERT(false, msg.c_str());
    }
}

std::shared_ptr<Camera> Renderer::GetCamera(std::string_view renderSceneName)
{
    auto iter = _renderScenes.find(std::string(renderSceneName));
    if (iter != _renderScenes.end())
    {
        auto scene = iter->second;
        return scene->GetCamera();
    }
    else
    {
        std::wstring msg = L"Renderer::GetRenderSceneImage: RenderSceneName '" +
                           std::wstring(renderSceneName.begin(), renderSceneName.end()) + L"' is not registered.";
        ASSERT(false, msg.c_str());
    }
}

void Renderer::InitializeImgui()
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors             = 200;
    desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    UmDevice.GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(_imguiDescriptorHeap.GetAddressOf()));
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

        config.MergeMode        = true;
        config.PixelSnapH = true;

        ImFontAtlas* atlas    = io.Fonts;
        ImFont*      iconFont = atlas->AddFontFromFileTTF(fontPath.string().c_str(), 15.0f, &config, icons_ranges);
    }
    io.Fonts->Build();
    auto cpuHandle = _imguiDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    auto gpuHandle = _imguiDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    ImGui_ImplWin32_Init(Global::engineCore->App.GetHwnd());
    ImGui_ImplDX12_Init(UmDevice.GetDevice().Get(), static_cast<int>(SWAPCHAIN_BUFFER_COUNT),
                        UmDevice.GetBackBufferFormat(), _imguiDescriptorHeap.Get(), cpuHandle, gpuHandle);
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

    ID3D12DescriptorHeap* descriptorHeaps[] = {_imguiDescriptorHeap.Get()};
    UmDevice.GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), UmDevice.GetCommandList().Get());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault(nullptr, nullptr);
    }
}

// SWTODO : 수정해야함. 너무 임시야. 임구이에서 어떻게 해당이미지의 gpu handle을 반환할지?
//          뭐가 반환될지 어떻게 알지?
D3D12_GPU_DESCRIPTOR_HANDLE Renderer::SceneView(RenderScene* scene)
{    
    auto dest = _imguiDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    UINT offset = _currentImGuiImageIndex * UmDevice.GetCBVSRVUAVDescriptorSize();
    dest.ptr += offset;

    auto src  = scene->GetFinalImage();
    UmDevice.GetDevice()->CopyDescriptorsSimple(1, dest, src, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = _imguiDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    gpuHandle.ptr += offset;

    // next ImGUI Descriptor Index
    _currentImGuiImageIndex++;

    return gpuHandle;
}