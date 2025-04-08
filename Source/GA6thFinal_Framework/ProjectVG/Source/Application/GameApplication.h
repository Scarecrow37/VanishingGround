#pragma once
#include "UmFramework.h"

class GameApplication : public Application
{
public:
    GameApplication();
    virtual ~GameApplication() override;

    EditorManager* _editorManager = nullptr;
};
