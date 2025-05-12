#pragma once
/*
에디터 툴을 끄고 키는 메뉴
*/
class EditorMenuDebug : public EditorMenu
{
public:
    EditorMenuDebug() 
    {
        SetCallOrder(0);
        SetPath("Setting/Editor");
        SetLabel("");

        _isDebugMode = Global::editorModule->IsDebugMode();
    }
    virtual ~EditorMenuDebug() = default;
public:
    virtual void OnMenu() override;

private:
    bool _isDebugMode = false;
};

class EditorMenuStyleEditor : public EditorMenu
{
public:
    EditorMenuStyleEditor()
    {
        SetCallOrder(0);
        SetPath("Setting/Editor");
        SetLabel("");
    }
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
    EditorMenuFileSystemSetting()
    {
        SetCallOrder(0);
        SetPath("Setting/FileSystem");
        SetLabel("");
    }
    virtual ~EditorMenuFileSystemSetting() = default;

public:
    virtual void OnMenu() override;
    virtual void OnTickGui() override;

private:
    bool _isOpenGui = false;
};