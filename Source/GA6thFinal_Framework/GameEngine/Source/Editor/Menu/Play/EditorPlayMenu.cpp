#include "pch.h"

using namespace Global;

void EditorPlayMenu::OnMenu() 
{
    static const std::string play = std::format("{} Play", EditorIcon::ICON_PLAY);
    if (true == ImGui::Button(play.c_str()))
    {
        editorModule->PlayMode.Play();
    }
    static const std::string pause = std::format("{} Pause", EditorIcon::ICON_PAUSE);
    if (true == ImGui::Button(pause.c_str()))
    {
        editorModule->PlayMode.Pause();
    }
    static const std::string stop = std::format("{} Stop", EditorIcon::ICON_STOP);
    if (true == ImGui::Button(stop.c_str()))
    {
        editorModule->PlayMode.Stop();
    }
}
