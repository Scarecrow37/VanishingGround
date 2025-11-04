#pragma once

class EditorTool;
class EditorMenu;
class EditorDockWindow;
class EditorModule;
class EditorPopupBoxSystem;

#ifndef _SCRIPTS_PROJECT
namespace Global
{
#ifdef _UMEDITOR
    extern EditorModule* editorModule;
#else
    struct NotEditorModule
    {
        NotEditorModule()  = default;
        ~NotEditorModule() = default;

        EditorModule* operator->()
        {
            assert(!"에디터 빌드만 접근 가능합니다.");
            __debugbreak(); // 에디터 아닌데 접근하면 안됨.
            UmApplication.Quit();
            return nullptr;
        }
        EditorModule& operator*()
        {
            assert(!"에디터 빌드만 접근 가능합니다.");
            __debugbreak(); // 에디터 아닌데 접근하면 안됨.
            EditorModule& err = reinterpret_cast<EditorModule&>(*this);
            UmApplication.Quit();
            return err;
        }
        EditorModule& operator=(EditorModule* rhs)
        {
            assert(!"에디터 빌드만 접근 가능합니다.");
            __debugbreak(); // 에디터 아닌데 접근하면 안됨.
            EditorModule& err = reinterpret_cast<EditorModule&>(*this);
            UmApplication.Quit();
            return err;
        }
        operator EditorModule*()
        {
            assert(!"에디터 빌드만 접근 가능합니다.");
            __debugbreak(); // 에디터 아닌데 접근하면 안됨.
            UmApplication.Quit();
            return nullptr;
        }
    };
    extern NotEditorModule editorModule;
#endif // _UMEDITOR
} // namespace Global
#endif

 class EditorModule 
     : public IAppModule
     , public File::FileEventSubscriber
 {
     friend class Application;
     friend class EditorTool;
 private:
     EditorModule();
     ~EditorModule();

 private:
     void PreInitialize() override;
     void ModuleInitialize() override;

     void PreUnInitialize() override;
     void ModuleUnInitialize() override;

 private:
     bool SaveSetting(const File::Path& path);
     bool LoadSetting(const File::Path& path);

 public:
     void Update();

 public:
     /// <summary>
     /// 팝업 박스를 엽니다.
     /// </summary>
     /// <param name="name">팝업 박스의 타이틀</param>
     /// <param name="content">팝업 박스에 나타낼 내용이 구현된 함수</param>
     void OpenPopupBox(const std::string& name, std::function<void()> content);

     /// <summary>
     /// 팝업 박스를 엽니다.
     /// </summary>
     /// <param name="name">팝업 박스의 타이틀</param>
     /// <param name="size">팝업 박스의 초기 지정 사이즈</param>
     /// <param name="flags">팝업 박스에 적용할 플래그</param>
     /// <param name="content">팝업 박스에 나타낼 내용이 구현된 함수</param>
     void OpenPopupBoxEx(const std::string& name, ImVec2 size, int flags, std::function<void()> content);

     /// <summary>
     /// Gui레이아웃을 처음 킨 레이아웃으로 초기화합니다.
     /// </summary>
     void ResetGuiLayout();

     /// <summary>
     /// Gui레이아웃을 프로젝트의 마지막 세팅 값으로 되돌립니다.
     /// </summary>
     void UndoGuiLayout();

     /// <summary>
     /// 지정된 포커스 영역이 없는지 확인합니다.
     /// </summary>
     /// <returns>지정된 포커스 영역이 하나도 없으면 true, 하나 이상 있으면 false를 반환합니다.</returns>
     bool IsFocusAreaEmpty() const;

     /// <summary>
     /// 해당 ID의 Label에 포커스 영역이 지정되어 있는지 확인합니다.
     /// </summary>
     /// <param name="id">포커스 영역의 Label값 입니다.</param>
     /// <returns></returns>
     bool IsFocusedArea(const char* id) const;

     /// <summary>
     /// 문자열에 해당하는 포커스 영역을 지정합니다.
     /// </summary>
     /// <param name="id">포커스 영역의 Label값 입니다.</param>
     void SetFocusArea(const char* id);

     /// <summary>
     /// 해당 문자열의 포커스 영역을 해제합니다.
     /// </summary>
     /// <param name="id">포커스 영역의 Label값 입니다.</param>
     void UnsetFocusArea(const char* id);

     /// <summary>
     /// 에디터의 디버그 모드 여부를 설정합니다.
     /// </summary>
     /// <param name="v">디버그 모드 사용 여부입니다.</param>
     inline void SetDebugMode(bool v) { _isDebug = v; }

     /// <summary>
     /// 에디터가 디버그 모드인지 확인합니다.
     /// </summary>
     /// <returns></returns>
     inline bool IsDebugMode() const { return _isDebug; }

     inline EditorGuiSystem&        GetDockWindowSystem() { return _guiSystem; }
     inline EditorPopupBoxSystem&   GetPopupBoxSystem() { return _popupBoxSystem; }
     
 private:
     /* 기본 스타일 설정 */
     void SetGuiThemeStyle();
     /* 프로젝트 세이브 요청을 처리할 동작을 구현 */
     virtual void OnRequestedSave() override;
     /* 프로젝트 로드 요청을 처리할 동작을 구현 */
     virtual void OnRequestedLoad() override;

 private:
     bool _isDebug = false;                         // 디버그 모드 여부

     std::string _imGuiIniDataFromIniFile;          // ImGui 설정 데이터 (기본 ini 파일에서 불러온 값)
     std::string _imGuiIniDataFromSetting;          // ImGui 설정 데이터 (에디터 세팅에서 불러온 값)

     EditorGuiSystem _guiSystem;                    // 에디터 도킹 윈도우 시스템
     EditorPopupBoxSystem _popupBoxSystem;          // 에디터 모달 팝업 시스템

     std::unordered_set<ImGuiID> _focusAreaList;    // ImGui 락 아이디들

     std::queue<std::function<void()>> _eventQueue; // 이벤트 큐

     bool _isFirstTick = true;

 public:
    //플레이 모드 관리용
    class EditorPlayMode
    {
    public:
        EditorPlayMode();
        ~EditorPlayMode();

        constexpr bool IsPlay() const{ return _isPlay; }
        bool IsPause() const { return _isPause; }

        void Play();
        void Pause();
        void Stop();
        void SetPlayModeColor();
        void SetPlayModeColor(ImVec4 (&playModeColors)[ImGuiCol_COUNT]);
        void DefaultPlayModeColor();

    private:
        #ifdef _UMEDITOR
        bool _isPlay = false;
        #else
        static constexpr bool _isPlay = true;
        #endif
        std::vector<File::Guid>  _playSceneGuids;
        ImVec4      _playModeColors[ImGuiCol_COUNT];
        bool        _isPause;
    }
    PlayMode;

    //빌드 매니저
    class EditorBuildSystem
    {
    public:
        static constexpr const char* PROJECT_EXE_FOLDER_DEBUG    = "..\\bin-Debug";
        static constexpr const char* PROJECT_EXE_FOLDER_RELEASE  = "..\\bin-Release";
        #ifdef _DEBUG
        static constexpr const char* PROJECT_EXE_FOLDER = PROJECT_EXE_FOLDER_DEBUG;
        #else
        static constexpr const char* PROJECT_EXE_FOLDER = PROJECT_EXE_FOLDER_RELEASE;
        #endif

        static constexpr const char* PROJECT_BUILD_BATCH_DEBUG   = "..\\GameEngine\\project_build_debug.bat";
        static constexpr const char* PROJECT_BUILD_BATCH_RELEASE = "..\\GameEngine\\project_build_release.bat";
        #ifdef _DEBUG
        static constexpr const char* PROJECT_BUILD_BATCH_FILE = PROJECT_BUILD_BATCH_DEBUG;
        #else
        static constexpr const char* PROJECT_BUILD_BATCH_FILE = PROJECT_BUILD_BATCH_RELEASE;
        #endif
    public:
        EditorBuildSystem();
        ~EditorBuildSystem();

        bool BuildProject(std::string_view outPath);

    }
    BuildSystem;
 };

