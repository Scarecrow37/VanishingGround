#pragma once
#include "UmFramework.h"

/*
에디터 툴을 끄고 키는 메뉴
*/
class EditorMenuDebug : public EditorMenu
{
public:
    EditorMenuDebug() 
    {
        SetCallOrder(0);
        SetLabel("Setting/Editor");

        _isDebugMode = Global::editorManager->IsDebugMode();
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
        SetLabel("Setting/Editor");
    }
    virtual ~EditorMenuStyleEditor() = default;
public:
    virtual void OnMenu() override;
    virtual void OnTickGui() override;
private:
    bool _isOpenStyleEditor = false;
};