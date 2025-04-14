#pragma once
#include "UmFramework.h"

/*
에디터 툴을 끄고 키는 메뉴
*/
class EditorMenuDebug : public EditorMenuLeaf
{
public:
    EditorMenuDebug() 
    {
        SetCallOrder(0);
        SetMenuPath("Setting/Editor");
        SetLabel("Debug");

        SetToggleValue(&_isDebugMode);
    }
    virtual ~EditorMenuDebug() = default;
public:
    virtual void OnSelected() override;
private:
    bool _isDebugMode = false;
};

class EditorMenuStyleEditor : public EditorMenuLeaf
{
public:
    EditorMenuStyleEditor()
    {
        SetCallOrder(0);
        SetMenuPath("Setting/Editor");
        SetLabel("Style");

        SetToggleValue(&_isOpenStyleEditor);
    }
    virtual ~EditorMenuStyleEditor() = default;
public:
    virtual void OnSelected() override;
    virtual void OnTickGui() override;
private:
    bool _isOpenStyleEditor = false;
};