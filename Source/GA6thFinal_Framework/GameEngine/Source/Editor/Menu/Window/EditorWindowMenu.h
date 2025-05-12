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
    virtual void OnMenu() override;

private:
    EditorDockWindow* _focusWindow = nullptr;
};