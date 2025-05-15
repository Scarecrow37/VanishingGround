#pragma once
/*
에디터 툴을 끄고 키는 메뉴
*/
class EditorMenuEditorSetting : public EditorMenu
{
public:
    EditorMenuEditorSetting()  = default;
    virtual ~EditorMenuEditorSetting() = default;

public:
    virtual void OnMenu() override;
    virtual void OnTickGui() override;

private:
    bool _isDebugMode = false;
    bool _isOpenGui   = false;
};

class EditorMenuFileSystemSetting : public EditorMenu
{
public:
    EditorMenuFileSystemSetting()          = default;
    virtual ~EditorMenuFileSystemSetting() = default;

public:
    virtual void OnMenu() override;
    virtual void OnTickGui() override;

private:
    bool _isOpenGui = false;
};