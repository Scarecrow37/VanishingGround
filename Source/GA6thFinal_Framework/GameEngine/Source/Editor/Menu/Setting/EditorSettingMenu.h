#pragma once
/*
에디터 툴을 끄고 키는 메뉴
*/
class EditorMenuDebug : public EditorMenu
{
public:
    EditorMenuDebug();
    virtual ~EditorMenuDebug() = default;
public:
    virtual void OnMenu() override;

private:
    bool _isDebugMode = false;
};

class EditorMenuStyleEditor : public EditorMenu
{
public:
    EditorMenuStyleEditor()          = default;
    virtual ~EditorMenuStyleEditor() = default;
public:
    virtual void OnMenu() override;
    virtual void OnTickGui() override;
private:
    bool _isOpenGui = false;
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