#include "pch.h"

using namespace Global;

void EditorPlayMenu::OnMenu() 
{
    static const std::string play = std::format("{} Play", EditorIcon::ICON_PLAY);
    if (true == ImGui::Button(play.c_str()))
    {
        editorModule->PlayMode.Play();
    }
    static const std::string yesPause = std::format("{} Pause", EditorIcon::ICON_PLAY);
    static const std::string noPause = std::format("{} Pause", EditorIcon::ICON_PAUSE);
    bool pushPause = false;
    if (true == editorModule->PlayMode.IsPause())
    {
        pushPause = ImGui::Button(yesPause.c_str());
    }
    else
    {
        pushPause = ImGui::Button(noPause.c_str());
    }
    if (pushPause)
    {
        editorModule->PlayMode.Pause();
    }
    static const std::string stop = std::format("{} Stop", EditorIcon::ICON_STOP);
    if (true == ImGui::Button(stop.c_str()))
    {
        editorModule->PlayMode.Stop();
    }
}
