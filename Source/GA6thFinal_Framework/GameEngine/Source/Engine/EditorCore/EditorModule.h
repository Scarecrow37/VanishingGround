#pragma once
#include "Setting/EditorSetting.h"

class EditorTool;
class EditorModule;
class EditorMenuBar;
class EditorPopupBoxSystem;

#ifndef _SCRIPTS_PROJECT
namespace Global
{
#ifdef _UMEDITOR
    extern EditorModule* editorModule;
#else
    struct NotEditorModule
    {
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
    };
    extern NotEditorModule editorModule;
#endif // _UMEDITOR  
}
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

 public:
     /* 에디터 디버그 모드 */
     inline void SetDebugMode(bool v) { _setting.IsDebugMode = v; }
     inline bool IsDebugMode() { return _setting.IsDebugMode; }

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
     EditorSetting             _setting;            // 에디터 세팅 데이터
     EditorDockWindowSystem    _dockWindowSystem;   // 에디터 도킹 윈도우 시스템
     EditorPopupBoxSystem      _popupBoxSystem;     // 에디터 모달 팝업 시스템
 };
