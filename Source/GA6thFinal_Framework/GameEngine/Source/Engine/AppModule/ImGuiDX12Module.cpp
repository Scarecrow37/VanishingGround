#include "pch.h"
#include "ImGuiDX12Module.h"

void ImGuiDX12Module::PreInitialize() {

}

void ImGuiDX12Module::ModuleInitialize()
{
    //if (device == NULL)
    //{
    //    assert(!"device가 NULL입니다. ImguiModule::SetDevice를 설정해주세요.");
    //    return;
    //}

    ////device->GetImmediateContext(&device_context);

    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO();
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    //// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    //ImGuiStyle& style = ImGui::GetStyle();
    //if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    //{
    //    style.WindowRounding = 0.0f;
    //    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    //}

    //ImFontConfig fontConfig{};
    //ImFont* mainFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 20.0f, &fontConfig, io.Fonts->GetGlyphRangesKorean());
    //
    //static const ImWchar icons_ranges[] = {0xf000, 0xf3ff, 0}; // FontAwesome 유니코드 범위
    //ImFontConfig         config;
    //config.MergeMode = true; // 기존 폰트와 병합
    //ImFont* iconFont = io.Fonts->AddFontFromFileTTF(
    //    "Assets/Font Awesome 6 Free-Regular-400.ttf", 15.0f, &config, icons_ranges);
    //io.Fonts->Build();

    //ImGui_ImplWin32_Init(Global::engineCore->App.GetHwnd());
    ////ImGui_ImplDX12_Init(device.Get(), device_context.Get());
}

void ImGuiDX12Module::PreUnInitialize()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    //device_context.Reset();
    device.Reset();
}

void ImGuiDX12Module::ModuleUnInitialize()
{

}

void ImGuiDX12Module::ImguiBegin()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiDX12Module::ImguiEnd()
{
    ImGuiIO& io = ImGui::GetIO();

    //RECT rect;
    //if (GetClientRect(Application::GetHwnd(), &rect)) 
    //{
    //    POINT size;
    //    size.x = rect.right - rect.left;
    //    size.y = rect.bottom - rect.top;
    //    io.DisplaySize = ImVec2((float)size.x, size.y);
    //}

    ImGui::Render();
    //ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault(nullptr, nullptr);
    }
    
    
}
