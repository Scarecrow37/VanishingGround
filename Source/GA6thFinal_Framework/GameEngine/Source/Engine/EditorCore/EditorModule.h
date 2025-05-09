#pragma once
#include "Setting/EditorSetting.h"

class EditorTool;
class EditorModule;
class EditorMenuBar;
class EditorPopupBoxSystem;

#ifndef _SCRIPTS_PROJECT
namespace Global
{
    extern EditorModule* editorModule;
}
#endif

template <typename T>
concept IsEditorGui = std::is_base_of_v<EditorGui, T>;

template <typename T>
concept IsEditorTool = IsEditorGui<T> && std::is_base_of_v<EditorTool, T>;

template <typename T>
concept IsEditorMenu = IsEditorGui<T> && std::is_base_of_v<EditorMenu, T>;

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
     template <IsEditorTool T>
     void AddDockSpace()
     {

     }

     /* 툴을 등록합니다. */
     template <IsEditorGui T>
     void RegisterEditorObject()
     {
         if constexpr (IsEditorTool<T>)
         {
             _mainDockSpace->RegisterTool<T>();
         }
         else if constexpr (IsEditorMenu<T>)
         {
             _mainMenuBar->RegisterMenu<T>();
         }
     }

     /* 툴을 가져옵니다. */
     template <IsEditorGui T>
     T* GetEditorObject()
     {
         if constexpr (IsEditorTool<T>)
         {
             return _mainDockSpace->GetTool<T>();
         }
         else if constexpr (IsEditorMenu<T>)
         {
             return _mainMenuBar->GetMenu<T>();
         }
         return nullptr;
     }

     void OpenPopupBox(const std::string& name, std::function<void()> content) 
     {
         _PopupBox->OpenPopupBox(name, content); 
     }

 public:
     /* */
     inline EditorMenuBar* GetMainMenuBar() { return _mainMenuBar; }

     /* */
     inline EditorDockSpace* GetMainDockSpace() { return _mainDockSpace; }

     /* 에디터 디버그 모드 */
     inline void SetDebugMode(bool v) { _setting.IsDebugMode = v; }
     inline bool IsDebugMode() { return _setting.IsDebugMode; }
 private:
     /* 기본 스타일 설정 */
     void SetGuiThemeStyle();
     /* 프로젝트 세이브 요청을 처리할 동작을 구현 */
     virtual void OnRequestedSave() override;
     /* 프로젝트 로드 요청을 처리할 동작을 구현 */
     virtual void OnRequestedLoad() override;
 private:
     EditorSetting          _setting;

     EditorPopupBoxSystem*  _PopupBox;                          // 에디터 모달 팝업
     EditorMenuBar*         _mainMenuBar;                       // 에디터 메뉴 바
     EditorDockSpace*       _mainDockSpace;                     // 에디터 도킹 스페이스

     std::unordered_map<std::string, EditorDockSpace*> _editorToolTable; // 검색용 툴 컨테이너
     std::vector<EditorTool*> _editorToolList; // 에디터 툴 리스트
     std::vector<EditorMenu*> _editorMenuList; // 에디터 메뉴 리스트
 };
