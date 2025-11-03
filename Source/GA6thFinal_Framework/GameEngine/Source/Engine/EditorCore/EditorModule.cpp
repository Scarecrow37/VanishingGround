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
    _guiSystem.OnStartGui();

    UmFileSystem.RegisterFileEventSubscriber(this);
}

void EditorModule::PreUnInitialize() 
{
}

void EditorModule::ModuleUnInitialize()
{
    // 파괴 직전 함수 필요하면 추가
    _popupBoxSystem.OnEndGui();
    _guiSystem.OnEndGui();

    _popupBoxSystem.Clear();
    _guiSystem.Clear();
}

bool EditorModule::SaveSetting(const File::Path& path)
{
    File::Path generic = path.generic_string();

    std::ofstream fout(generic);
    if (true == fout.is_open())
    {
        YAML::Node node;
        node["debug"]        = _isDebug;
        node["GuiToolData"]  = _guiSystem.SaveGuiSettingToMemory();
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
        auto [node, result] = YAMLHelper::SafeLoadFile(generic);
        if (result)
        {
            if (false == node.IsNull())
            {
                if (node["debug"])
                    _isDebug = node["debug"].as<bool>();

                if (node["imGuiIniData"])
                    _imGuiIniDataFromSetting = node["imGuiIniData"].as<std::string>();

                if (node["GuiToolData"])
                    _guiSystem.LoadGuiSettingFromMemory(node["GuiToolData"]);

                UndoGuiLayout();

                return true;
            }
        }
    }
    SaveSetting(generic);
    return false;
}

void EditorModule::Update()
{
    _popupBoxSystem.OnTickGui();
    _guiSystem.OnTickGui();

    bool isPopupEmpty = _popupBoxSystem.IsEmpty();
    if (false == isPopupEmpty)
        ImGui::BeginDisabled();
    
    _guiSystem.OnDrawGui();

    if (false == isPopupEmpty)
        ImGui::EndDisabled();

    _popupBoxSystem.OnDrawGui();

    if (false == _eventQueue.empty())
    {
        auto func = _eventQueue.front();
        if (func)
        {
            func();
        }
        _eventQueue.pop();
    }
    if (true == _isFirstTick)
    {
        _imGuiIniDataFromIniFile = ImGui::SaveIniSettingsToMemory();
        _isFirstTick = false;
    }
}

void EditorModule::OpenPopupBox(const std::string& name, std::function<void()> content) 
{
    auto* popup = _popupBoxSystem.OpenPopupBox(name, content);
}

void EditorModule::OpenPopupBoxEx(const std::string& name, ImVec2 size, int flags, std::function<void()> content) 
{
    auto* popup = _popupBoxSystem.OpenPopupBox(name, content);
    if (popup)
    {
        popup->SetFlags(flags);
        popup->SetSize(size);
    }
}

void EditorModule::ResetGuiLayout() 
{
    _eventQueue.push([this]() {
        ImGui::ClearIniSettings();
        ImGui::LoadIniSettingsFromMemory(_imGuiIniDataFromIniFile.c_str());
        ImGui::GetIO().IniFilename = nullptr; // 인스턴스가 파괴될 때까지 저장하지 않음
    });
}

void EditorModule::UndoGuiLayout() 
{
    _eventQueue.push([this]() {
        ImGui::ClearIniSettings();
        ImGui::LoadIniSettingsFromMemory(_imGuiIniDataFromSetting.c_str());
        ImGui::GetIO().IniFilename = nullptr; // 인스턴스가 파괴될 때까지 저장하지 않음
    });
}

bool EditorModule::IsFocusAreaEmpty() const
{
    return _focusAreaList.empty();
}

bool EditorModule::IsFocusedArea(const char* id) const
{
    ImGuiID imguiId = ImHashStr(id);
    if (_focusAreaList.contains(imguiId))
    {
        return true;
    }
    return false;
}

void EditorModule::SetFocusArea(const char* id)
{
    ImGuiID imguiId = ImHashStr(id);
    if (!_focusAreaList.contains(imguiId))
    {
        _focusAreaList.insert(imguiId);
    }
}

void EditorModule::UnsetFocusArea(const char* id)
{
    ImGuiID imguiId = ImHashStr(id);
    _focusAreaList.erase(imguiId);
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
    auto& path = UmFileSystem.GetProjectSettingPath();
    SaveSetting(path / name);
}

void EditorModule::OnRequestedLoad() 
{
    File::Path name = L"EditorModule.UmSetting";
    auto& path = UmFileSystem.GetProjectSettingPath();
    LoadSetting(path / name);
}

EditorModule::EditorPlayMode::EditorPlayMode() 
    : _isPause(false)
{
}

EditorModule::EditorPlayMode::~EditorPlayMode() 
{
}

void EditorModule::EditorPlayMode::Play()
{
    if (_isPause)
    {
        Pause();
    }
    if (false == _isPlay)
    {
        UmCommandManager.Clear();

        Scene* scene = UmSceneManager.GetMainScene();
        if (nullptr != scene)
        {
            File::Path path = (std::string)scene->Path;         
            _playSceneGuid = path.ToGuid();

            auto writePath = std::filesystem::relative(path, UmFileSystem.GetAssetPath()).parent_path();
            UmSceneManager.WriteSceneToFile(*scene, writePath.string(), true);
            UmSceneManager.LoadScene(path.string()); 
            SetPlayModeColor();
            UmTime.TimeScale = 1.f;
            #ifdef _UMEDITOR
            _isPlay = true;
            #endif // _UMEDITOR
        }
    }
}

void EditorModule::EditorPlayMode::Pause() 
{
    if (_isPlay)
    {
        if (true == _isPause)
        {
            UmTime.TimeScale = 1.f;
            _isPause = false;
        }
        else
        {
            UmTime.TimeScale = 0.f;
            _isPause = true;
        }
    }
}

void EditorModule::EditorPlayMode::Stop() 
{
    if (true == _isPlay)
    {
        if (_isPause)
        {
            Pause();
        }
        UmCommandManager.Clear();
        for (const auto& object : ESceneManager::Engine::GetRuntimeObjects())
        {
            if (object)
            {
                if (object->GetOwnerSceneName() == ESceneManager::DONT_DESTROY_ON_LOAD_SCENE_NAME)
                {
                    GameObject::Destroy(object.get());
                }
            }
        }
        UmSceneManager.LoadScene(_playSceneGuid.ToPath().string());
        Global::editorModule->SetGuiThemeStyle();
        UmTime.TimeScale = 1.f;

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

EditorModule::EditorBuildSystem::EditorBuildSystem() 
{

}

EditorModule::EditorBuildSystem::~EditorBuildSystem() 
{

}

bool EditorModule::EditorBuildSystem::BuildProject(std::string_view outPath)
{
    using namespace u8_literals;
    namespace fs = std::filesystem;

    if (false == UmFileSystem.IsLoadedProject())
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, u8"프로젝트를 열어주세요."_c_str);
    }

    fs::path batchPath = PROJECT_BUILD_BATCH_FILE;
    DWORD exitCode{};
    bool batchResult = dllUtility::RunBatchFile(batchPath.c_str(), &exitCode);
    if (false == batchResult)
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, u8"프로젝트 빌드 실패."_c_str);
        return false;
    }

    fs::path destPath = outPath;
    fs::path exePath  = PROJECT_EXE_FOLDER;
#ifdef _DEBUG
    fs::path scriptPath = EComponentFactory::Engine::SCRIPTS_DLL_DEBUG_PATH;
#else
    fs::path scriptPath = EComponentFactory::Engine::SCRIPTS_DLL_RELEASE_PATH;
#endif
    File::Path            rootPath = UmFileSystem.GetRootPath();
    std::vector<fs::path> pathStack;
    fs::create_directories(destPath);

    //스크립트 dll 복사
    for (const auto& entry : fs::directory_iterator(scriptPath))
    {
        if (fs::is_regular_file(entry.path()))
        {
            constexpr const wchar_t* extentions[] = {
                L".dll", 
                L".pdb"
            };
            bool isCopy = false;
            for (auto& extention : extentions)
            {
                isCopy |= extention == entry.path().extension();
                if (isCopy)
                {
                    break;
                }
            }
            if (true == isCopy)
            {
                fs::path copyPath = destPath / "bin" / entry.path().filename();
                fs::create_directories(copyPath.parent_path());
                fs::copy_file(entry.path(), copyPath, fs::copy_options::overwrite_existing);
            }
        }
    }

    //exe 파일 및 dll 복사
    pathStack.push_back(exePath);
    while (false == pathStack.empty())
    {
        fs::path curr = pathStack.back();
        pathStack.pop_back();
        for (const auto& entry : fs::directory_iterator(curr))
        {
            if (fs::is_regular_file(entry.path()))
            {
                fs::path copyPath = destPath / "bin" / entry.path().filename();
                fs::create_directories(copyPath.parent_path());
                fs::copy_file(entry.path(), copyPath, fs::copy_options::overwrite_existing);
            }
            else
            {
                pathStack.push_back(entry);
            }
        }
    }
    
    //리소스 복사
    pathStack.push_back(rootPath);
    while (false == pathStack.empty())
    {
        fs::path curr = pathStack.back();
        pathStack.pop_back();
        for (const auto& entry : fs::directory_iterator(curr))
        {
            if (fs::is_regular_file(entry.path()))
            {
                fs::path relative = entry.path().lexically_relative(rootPath);
                fs::path copyPath = destPath / "bin" / relative;
                fs::create_directories(copyPath.parent_path());
                fs::copy_file(entry.path(), copyPath, fs::copy_options::overwrite_existing);
            }
            else
            {
                pathStack.push_back(entry);
            }
        }
    }
    return true;
}
