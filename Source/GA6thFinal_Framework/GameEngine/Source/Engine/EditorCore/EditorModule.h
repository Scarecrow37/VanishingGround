#pragma once

class EditorTool;
class EditorMenu;
class EditorDockWindow;
class EditorModule;
class EditorPopupBoxSystem;
class EditorModule;

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
     , public File::FileEventNotifier
 {
     friend class Application;
     friend class EditorTool;
 private:
     EditorModule();
     ~EditorModule();

 public:
     void PreInitialize() override;
     void ModuleInitialize() override;

     void PreUnInitialize() override;
     void ModuleUnInitialize() override;

 private:
     bool SaveSetting(const File::Path& path);
     bool LoadSetting(const File::Path& path);

 public:
     void Update();

     bool IsLock();

 public:
     void OpenPopupBox(const std::string& name, std::function<void()> content);

     void ResetGuiLayout();

 public:
     /* 에디터 디버그 모드 */
     inline void SetDebugMode(bool v) { _isDebug = v; }
     inline bool IsDebugMode() { return _isDebug; }

     inline auto& GetDockWindowSystem() { return _dockWindowSystem; }
     inline auto& GetPopupBoxSystem()   { return _popupBoxSystem; }
     
 private:
     /* 기본 스타일 설정 */
     void SetGuiThemeStyle();
     /* 프로젝트 세이브 요청을 처리할 동작을 구현 */
     virtual void OnRequestedSave() override;
     /* 프로젝트 로드 요청을 처리할 동작을 구현 */
     virtual void OnRequestedLoad() override;

 private:
     bool _isDebug = false;
     std::string _imGuiIniData;   // ImGui 설정 데이터

     EditorGuiSystem            _dockWindowSystem;   // 에디터 도킹 윈도우 시스템
     EditorPopupBoxSystem       _popupBoxSystem;     // 에디터 모달 팝업 시스템

     bool _isFirstTick     = true;
     bool _isRefreshLayout = false;

 public:
    //플레이 모드 관리용
    class EditorPlayMode
    {
    public:
        EditorPlayMode();
        ~EditorPlayMode();

        constexpr bool IsPlay() const
        {
            return _isPlay;
        }
        void Play();
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
        File::Guid _playSceneGuid;
        ImVec4 _playModeColors[ImGuiCol_COUNT];
    }
    PlayMode;

 public:
     //커맨드 매니저
     ECommandManager CommandManager;

 };

#ifndef _SCRIPTS_PROJECT
#define UmCommandManager Global::editorModule->CommandManager
#endif