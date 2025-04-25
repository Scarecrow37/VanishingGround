#include "pch.h"
#include "Renderer.h"
#include "Shader.h"
#include "FrameResource.h"
#include "Model.h"
#include "UmScripts.h"

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

Renderer::Renderer() : _currnetState(0)
{
}

Renderer::~Renderer()
{
}

void Renderer::RegisterRenderQueue(MeshRenderer* component)
{
    auto iter = std::find_if(_components.begin(), _components.end(), [component](const auto& ptr) { return ptr == component; });

    if (iter != _components.end())
    {
        ASSERT(false, L"Renderer::RegisterRenderQueue : Already registered component.");
        return;
    }

    _components.push_back(component);
    // test code
    for (auto& meshRenerComps : _components)
    {
        _renderScenes["Test"]->RegisterOnRenderQueue(meshRenerComps);
    }
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
    std::shared_ptr<RenderScene> testRenderScene = std::make_shared<RenderScene>();
    testRenderScene->InitializeRenderScene();
    std::shared_ptr<PBRLitTechnique> pbrTech = std::make_shared<PBRLitTechnique>();
    testRenderScene->AddRenderTechnique("PBRLIT", pbrTech);
    _renderScenes["Test"] = testRenderScene;

}

void Renderer::Update()
{
    // 비활성된 컴포넌트 제거
    auto first = std::remove_if(_components.begin(), _components.end(), [](const auto& ptr) { return (!ptr->Enable || !ptr->gameObject->ActiveInHierarchy); });
    _components.erase(first, _components.end());

    UmMainCamera.Update();

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
}

D3D12_GPU_DESCRIPTOR_HANDLE Renderer::GetRenderSceneImage(std::string_view renderSceneName)
{
    auto iter = _renderScenes.find(std::string(renderSceneName));
    if (iter != _renderScenes.end())
    {
        auto& scene = iter->second;
        return scene->GetFinalImage();
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

        ImFontConfig config;
        config.MergeMode = true; // 기존 폰트와 병합

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

