#include "pch.h"
#include "EditorNodeMananger.h"

void EditorNodeMananger::Initialize()
{
}

void EditorNodeMananger::DrawGui() 
{
}

void EditorNodeMananger::UnInitialize() 
{
    ed::Config config;

    config.SettingsFile = "Blueprints.json";

    config.UserPointer = this;
}
