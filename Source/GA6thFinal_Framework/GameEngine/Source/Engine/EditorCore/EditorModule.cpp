#include "pch.h"
#include "EditorModule.h"
#include "EditorGui.h"
#include "EditorMenuBar.h"

EditorModule* Global::editorModule = nullptr;

EditorModule::EditorModule() 
    : _isDebugMode(false)
{
    Global::editorModule = this;
    _mainMenuBar = new EditorMenuBar;
    _mainDockSpace = new EditorDockSpace;
    _PopupBox = new EditorPopupBoxSystem;
}

EditorModule::~EditorModule()
{
    delete _mainMenuBar;
    delete _mainDockSpace;
    delete _PopupBox;
}

void EditorModule::ModuleInitialize()
{
    // 모듈 등록시 1회 호출
    SetGuiThemeStyle();
    _mainMenuBar->OnStartGui();
    _mainDockSpace->OnStartGui();

    File::Path filename  = L"editor.setting";
    File::Path directory = PROJECT_SETTING_PATH;
    LoadSetting(directory / filename);
}

void EditorModule::ModuleUnInitialize()
{ 
    //파괴 직전 함수 필요하면 추가
    _mainMenuBar->OnEndGui();
    _mainDockSpace->OnEndGui();

    File::Path filename  = L"editor.setting";
    File::Path directory = PROJECT_SETTING_PATH;

    SaveSetting(directory / filename);
}

bool EditorModule::SaveSetting(const File::Path& path)
{
    _setting.ToolStatus.clear();
    for (auto& [key, tool] : _mainDockSpace->GetRefToolTable())
    {
        EditorTool* editorTool = tool.get();
        if (nullptr != editorTool)
        {
            EditorToolStatus status;
            status.name      = key;
            status.IsVisible = editorTool->IsVisible();
            status.IsLock    = editorTool->IsLock();
            _setting.ToolStatus.push_back(status);
        }
    }

    auto setting = rfl::yaml::save(path.string(), _setting);
    if (false == setting)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool EditorModule::LoadSetting(const File::Path& path)
{
    auto setting = rfl::yaml::load<EditorSetting>(path.string());
    if (false == setting)
    {
        return false;
    }
    else
    {
        _setting = setting.value();

        for (auto& status : _setting.ToolStatus)
        {
            EditorTool* tool = _mainDockSpace->GetTool(status.name);
            if (nullptr != tool)
            {
                tool->SetVisible(status.IsVisible);
                tool->SetLock(status.IsLock);
            }
        }
        return true;
    }
}

void EditorModule::Update()
{
    bool isLock = IsLock();
    if (true == isLock)
        ImGui::BeginDisabled();

    /* ========GUI Update======== */ 
    _mainMenuBar->OnTickGui();
    _mainDockSpace->OnTickGui();
    _mainMenuBar->OnDrawGui();
    _mainDockSpace->OnDrawGui();
    /* =========================== */

    if (true == isLock)
        ImGui::EndDisabled();

    _PopupBox->OnDrawGui(); // 모달 팝업창 
}

bool EditorModule::IsLock()
{
    return (false == _PopupBox->IsEmpty());
}

void EditorModule::SetGuiThemeStyle()
{
    ImVec4 Colors[ImGuiCol_COUNT];
    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

    // Headers
    colors[ImGuiCol_Header] = ImVec4{ 0.1f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
    colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
}