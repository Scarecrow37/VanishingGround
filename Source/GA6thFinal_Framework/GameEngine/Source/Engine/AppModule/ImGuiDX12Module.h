#pragma once

class ImGuiDX12Module : public IAppModule
{
public:
    ImGuiDX12Module()          = default;
    virtual ~ImGuiDX12Module() = default;

    void ImguiBegin();
    void ImguiEnd();

private:
    void PreInitialize();
    void ModuleInitialize();

    void PreUnInitialize();
    void ModuleUnInitialize();

    static bool ImGuiWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

enum MouseXButton : int
{
    ImGuiMouseButton_XButton1 = 3, // XButton1 (뒤로 가기)
    ImGuiMouseButton_XButton2 = 4, // XButton2 (앞으로 가기)
};