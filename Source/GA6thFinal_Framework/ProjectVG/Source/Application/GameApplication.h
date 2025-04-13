#pragma once
#include "UmFramework.h"

class GameApplication : public Application
{
public:
    GameApplication();
    virtual ~GameApplication() override;

    EditorModule* _editorManager = nullptr;
};
