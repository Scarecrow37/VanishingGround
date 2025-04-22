#pragma once
#include "UmFramework.h"

class GameApplication : public Application
{
public:
    GameApplication();
    virtual ~GameApplication() override;

    EditorModule* _editorModule = nullptr;
};

// MySampleEditorTool.h
class MySampleEditorTool : public EditorTool
{
public:
    MySampleEditorTool();
    virtual ~MySampleEditorTool() = default;

public:
    /* Begin 호출 시 호출 */
    virtual void OnFrame();

    /* PopUp창 호출 성공 시 호출 */
    virtual void OnPopup();
};
