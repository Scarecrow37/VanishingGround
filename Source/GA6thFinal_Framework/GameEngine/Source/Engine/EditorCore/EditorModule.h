#pragma once

class EditorTool;
class EditorModule;
class EditorMenuBar;

#ifndef SCRIPTS_PROJECT
namespace Global
{
    extern EditorModule* editorManager;
}

#endif

template <typename T>
concept IsEditorBase = std::is_base_of_v<EditorBase, T>;

template <typename T>
concept IsEditorTool = IsEditorBase<T> && std::is_base_of_v<EditorTool, T>;

template <typename T>
concept IsEditorMenu = IsEditorBase<T> && std::is_base_of_v<EditorMenu, T>;

 class EditorModule : public IAppModule
 {
     friend class Application;
     friend class EditorTool;
 private:
     EditorModule();
     ~EditorModule();
 public:
     void PreInitialize() override {}
     void ModuleInitialize() override;

     void PreUnInitialize() override {}
     void ModuleUnInitialize() override;
 public:
     /* InitImGui 직후 호출 */
     void Update();
 public:
     /* 툴을 등록합니다. */
     template <IsEditorBase T>
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
     template <IsEditorBase T>
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

 public:
     /* */
     inline EditorMenuBar* GetMainMenuBar() { return _mainMenuBar; }

     /* */
     inline EditorDockSpace* GetMainDockSpace() { return _mainDockSpace; }

     /* 에디터 디버그 모드 */
     inline void SetDebugMode(bool v) { _isDebugMode = v; }
     inline bool IsDebugMode() { return _isDebugMode; }
 private:
     void SetGuiThemeStyle();
 private:
     bool _isDebugMode;                                         // 에디터 디버그 모드 여부(에디터관련 정보 출력)
     EditorMenuBar* _mainMenuBar;                               // 에디터 메뉴 바
     EditorDockSpace* _mainDockSpace;                           // 에디터 도킹 스페이스
};
