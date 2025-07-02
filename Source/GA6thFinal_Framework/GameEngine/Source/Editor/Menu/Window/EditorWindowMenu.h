#pragma once

class EditorDockWindow;

/*
에디터 툴을 끄고 키는 메뉴
*/
class EditorMenuTools : public EditorMenu
{
public:
    EditorMenuTools(EditorDockWindow* focusWindow);
    virtual ~EditorMenuTools() = default;

public:
    virtual void OnStartGui() override;
    virtual void OnMenu() override;

private:
    EditorDockWindow* _focusWindow = nullptr;

    std::unordered_map<std::string, EditorDockWindow*> _dockWindowTable; // 도킹 윈도우 테이블
    std::unordered_map<std::string, EditorTool*> _editorToolTable; // 에디터 툴 테이블
};