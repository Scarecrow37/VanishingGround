#include "pch.h"
#include "EditorModule.h"

#ifdef _UMEDITOR
EditorModule* Global::editorModule = nullptr;
#else
Global::NotEditorModule Global::editorModule;
#endif

EditorModule::EditorModule() 
{
    Global::editorModule = this;
}

EditorModule::~EditorModule()
{
}

void EditorModule::PreInitialize() {}

void EditorModule::ModuleInitialize()
{
    // 모듈 등록시 1회 호출
    SetGuiThemeStyle();
    _popupBoxSystem.OnStartGui();
    _dockWindowSystem.OnStartGui();

    UmFileSystem.RegisterFileEventNotifier(this);
}

void EditorModule::PreUnInitialize() 
{
}

void EditorModule::ModuleUnInitialize()
{
    // 파괴 직전 함수 필요하면 추가
    _popupBoxSystem.OnEndGui();
    _dockWindowSystem.OnEndGui();
}

bool EditorModule::SaveSetting(const File::Path& path)
{
    File::Path generic = path.generic_string();

    std::ofstream fout(generic);
    if (true == fout.is_open())
    {
        YAML::Node node;
        node["debug"]        = _isDebug;
        node["GuiToolData"]  = _dockWindowSystem.SaveGuiSettingToMemory();
        node["imGuiIniData"] = ImGui::SaveIniSettingsToMemory();

        fout << node;
        fout.close();

        return true;
    }
    return false;
}

bool EditorModule::LoadSetting(const File::Path& path)
{
    File::Path generic = path.generic_string();
    if (true == std::filesystem::exists(generic))
    {
        YAML::Node node = YAML::LoadFile(generic.string());
        if (false == node.IsNull())
        {
            if (node["debug"])
                _isDebug = node["debug"].as<bool>();

            if (node["imGuiIniData"])
                _imGuiIniData = node["imGuiIniData"].as<std::string>();

            if (node["GuiToolData"])
                _dockWindowSystem.LoadGuiSettingFromMemory(node["GuiToolData"]);

            ResetGuiLayout();

            return true;
        }
    }
    SaveSetting(generic);
    return false;
}

void EditorModule::Update()
{
    bool isLock = IsLock();

    if (true == isLock)
        ImGui::BeginDisabled();

    /* ========GUI Update======== */ 
    _popupBoxSystem.OnTickGui();

    _dockWindowSystem.OnTickGui();
    _dockWindowSystem.OnDrawGui();
    /* =========================== */

    if (true == isLock)
        ImGui::EndDisabled();

    _popupBoxSystem.OnDrawGui();

    if (true == _isRefreshLayout)
    {
        _isRefreshLayout = false;
        ImGui::LoadIniSettingsFromMemory(_imGuiIniData.c_str());
    }
    if (true == _isFirstTick)
    {
        _imGuiIniData = ImGui::SaveIniSettingsToMemory();
        _isFirstTick = false;
    }
}

bool EditorModule::IsLock()
{
    return (false == _popupBoxSystem.IsEmpty());
}

void EditorModule::OpenPopupBox(const std::string& name, std::function<void()> content) 
{
    _popupBoxSystem.OpenPopupBox(name, content);
}

void EditorModule::ResetGuiLayout() 
{
    _isRefreshLayout = true;
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

    // DragDrop
    colors[ImGuiCol_DragDropTarget] = ImVec4{0.2f, 0.6f, 0.4f, 1.0f};

    PlayMode.SetPlayModeColor(colors);
    PlayMode.DefaultPlayModeColor();
}

void EditorModule::OnRequestedSave()
{
    File::Path name = L"EditorModule.UmSetting";
    auto& path = UmFileSystem.GetSettingPath();
    SaveSetting(path / name);
}

void EditorModule::OnRequestedLoad() 
{
    File::Path name = L"EditorModule.UmSetting";
    auto& path = UmFileSystem.GetSettingPath();
    LoadSetting(path / name);
}

EditorModule::EditorPlayMode::EditorPlayMode() 
{
    
}

EditorModule::EditorPlayMode::~EditorPlayMode() 
{

}

void EditorModule::EditorPlayMode::Play()
{
    if (false == _isPlay)
    {
        Scene* scene = UmSceneManager.GetMainScene();
        if (nullptr != scene)
        {
            File::Path path = (std::string)scene->Path;         
            _playSceneGuid = path.ToGuid();

            auto writePath = std::filesystem::relative(path, UmFileSystem.GetAssetPath()).parent_path();
            UmSceneManager.WriteSceneToFile(*scene, writePath.string(), true);
            for (const auto& object : ESceneManager::Engine::GetRuntimeObjects())
            {
                if (object)
                {
                    object->ActiveSelf = false; // 스크립트 이벤트 함수들 호출 방지용
                }           
            }
            UmSceneManager.LoadScene(path.string()); 
            SetPlayModeColor();

            #ifdef _UMEDITOR
            _isPlay = true;
            #endif // _UMEDITOR
        }
    }
}

void EditorModule::EditorPlayMode::Stop() 
{
    if (true == _isPlay)
    {
        for (const auto& object : ESceneManager::Engine::GetRuntimeObjects())
        {
            if (object)
            {
                object->ActiveSelf = false; //스크립트 이벤트 함수들 호출 방지용
                if (object->GetOwnerSceneName() == ESceneManager::DONT_DESTROY_ON_LOAD_SCENE_NAME)
                {
                    GameObject::Destroy(object.get()); 
                }        
            }
        }
        UmSceneManager.LoadScene(_playSceneGuid.ToPath().string());
        Global::editorModule->SetGuiThemeStyle();

        #ifdef _UMEDITOR
        _isPlay = false;
        #endif
    }
}

void EditorModule::EditorPlayMode::SetPlayModeColor() 
{
    auto& colors = ImGui::GetStyle().Colors;
    std::memcpy(&colors, &_playModeColors, sizeof(_playModeColors));
}

void EditorModule::EditorPlayMode::SetPlayModeColor(ImVec4 (&playModeColors)[ImGuiCol_COUNT]) 
{
    std::memcpy(&_playModeColors, &playModeColors, sizeof(_playModeColors));
}

void EditorModule::EditorPlayMode::DefaultPlayModeColor()
{
    // Headers
    _playModeColors[ImGuiCol_Header]        = ImVec4{0.09f, 0.17f, 0.22f, 1.0f};
    _playModeColors[ImGuiCol_HeaderHovered] = ImVec4{0.22f, 0.32f, 0.42f, 1.0f};
    _playModeColors[ImGuiCol_HeaderActive]  = ImVec4{0.13f, 0.19f, 0.25f, 1.0f};

    // Buttons
    _playModeColors[ImGuiCol_Button]        = ImVec4{0.13f, 0.18f, 0.23f, 1.0f};
    _playModeColors[ImGuiCol_ButtonHovered] = ImVec4{0.23f, 0.33f, 0.44f, 1.0f};
    _playModeColors[ImGuiCol_ButtonActive]  = ImVec4{0.15f, 0.22f, 0.28f, 1.0f};

    // Frame BG
    _playModeColors[ImGuiCol_FrameBg]        = ImVec4{0.14f, 0.19f, 0.24f, 1.0f};
    _playModeColors[ImGuiCol_FrameBgHovered] = ImVec4{0.25f, 0.35f, 0.46f, 1.0f};
    _playModeColors[ImGuiCol_FrameBgActive]  = ImVec4{0.18f, 0.26f, 0.33f, 1.0f};

    // Tabs
    _playModeColors[ImGuiCol_Tab]                = ImVec4{0.12f, 0.17f, 0.22f, 1.0f};
    _playModeColors[ImGuiCol_TabHovered]         = ImVec4{0.30f, 0.42f, 0.55f, 1.0f};
    _playModeColors[ImGuiCol_TabActive]          = ImVec4{0.22f, 0.32f, 0.42f, 1.0f};
    _playModeColors[ImGuiCol_TabUnfocused]       = ImVec4{0.12f, 0.17f, 0.22f, 1.0f};
    _playModeColors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.18f, 0.26f, 0.33f, 1.0f};

    // Title
    _playModeColors[ImGuiCol_TitleBg]          = ImVec4{0.12f, 0.17f, 0.22f, 1.0f};
    _playModeColors[ImGuiCol_TitleBgActive]    = ImVec4{0.13f, 0.18f, 0.24f, 1.0f};
    _playModeColors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.10f, 0.15f, 0.19f, 1.0f};

    // DragDrop
    _playModeColors[ImGuiCol_DragDropTarget] = ImVec4{0.1f, 0.4f, 0.65f, 1.0f};
}
