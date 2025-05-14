#include "pch.h"
#include "ImGuiDX12Module.h"
#include "UmScripts.h"

void ImGuiDX12Module::PreInitialize()
{
    UmRenderer.InitializeImgui();
}

void ImGuiDX12Module::ModuleInitialize()
{

}

void ImGuiDX12Module::PreUnInitialize()
{
    UmRenderer.PreUnInitializeImgui();
}

void ImGuiDX12Module::ModuleUnInitialize() {}

void ImGuiDX12Module::ImguiBegin()
{
    UmRenderer.ImguiBegin();
}

void ImGuiDX12Module::ImguiEnd()
{
    UmRenderer.ImguiEnd();
}
