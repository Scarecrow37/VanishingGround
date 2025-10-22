#include "pch.h"
#include "ImGuiDX12Module.h"

void ImGuiDX12Module::PreInitialize()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // IF using Docking Branch
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular
    // ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding              = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // 폰트 경로는 실제 폰트 파일로 바꿔주세요
    ImFontConfig fontConfig;
    fontConfig.OversampleH = 3;
    fontConfig.OversampleV = 3;
    fontConfig.PixelSnapH  = true;

    ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    builder.AddRanges(io.Fonts->GetGlyphRangesKorean());
    constexpr ImWchar romanRanges[] = {
        0x2160,
        0x218F, 
        0,
    };
    builder.AddRanges(romanRanges);
    builder.BuildRanges(&ranges);
    
    ImFont* mainFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 20.0f, &fontConfig, ranges.Data);

    std::string fontFileName = "Font Awesome 6 Free-Regular-400.ttf";
    auto        fontPath     = UmFileSystem.GetRootPath();
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
       
    auto device         = UmGraphics.GetDevice();
    auto descriptorHeap = UmGraphics.GetShaderResourceDescriptorHeap();
    auto cpuHandle      = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    auto gpuHandle      = descriptorHeap->GetGPUDescriptorHandleForHeapStart();

    ImGui_ImplWin32_Init(UmApplication.GetHwnd());
    ImGui_ImplDX12_Init(device, static_cast<int>(SWAPCHAIN_BUFFER_COUNT), DXGI_FORMAT_R8G8B8A8_UNORM, descriptorHeap, cpuHandle, gpuHandle);
}

void ImGuiDX12Module::ModuleInitialize()
{
    // ImGui 메세지 핸들러
    const MessageHandler msgHandler(ImGuiWinProc, 999);
    UmApplication.AddMessageHandler(msgHandler);
}

void ImGuiDX12Module::PreUnInitialize()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiDX12Module::ModuleUnInitialize()
{
}

bool ImGuiDX12Module::ImGuiWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ImGuiIO& io = ImGui::GetIO();
    switch (msg)
    {
        case WM_XBUTTONDOWN:
        case WM_XBUTTONDBLCLK: 
        {
            int button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? ImGuiMouseButton_XButton1 : ImGuiMouseButton_XButton2;
            io.AddMouseButtonEvent(button, true);                          // Press & Click 이벤트
            return 0;
        }
        case WM_XBUTTONUP: 
        {
            int button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? ImGuiMouseButton_XButton1 : ImGuiMouseButton_XButton2;
            io.AddMouseButtonEvent(button, false); // Release 이벤트
            return 0;
        }
    }
    return false;
}

void ImGuiDX12Module::ImguiBegin()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

void ImGuiDX12Module::ImguiEnd()
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Render();
    
    ID3D12DescriptorHeap*      descriptorHeaps[] = {UmGraphics.GetShaderResourceDescriptorHeap()};
    ID3D12GraphicsCommandList* commandList       = UmGraphics.GetCommandList();

    commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    auto backBuffer = UmGraphics.GetBackBufferHandle();
    commandList->OMSetRenderTargets(1, &backBuffer, FALSE, nullptr);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault(nullptr, nullptr);
    }
}
