#include "pch.h"

using namespace Global;

void EditorPlayMenu::OnMenu() 
{
    if (ImGui::BeginMenu("Play"))
    {
        static const std::string play = std::format("{} Play", EditorIcon::ICON_PLAY);
        if (ImGui::MenuItem(play.c_str()))
        {
            editorModule->PlayMode.Play();
        }

        static const std::string stop = std::format("{} Stop", EditorIcon::ICON_STOP);
        if (ImGui::MenuItem(stop.c_str()))
        {
            editorModule->PlayMode.Stop();
        }
        ImGui::EndMenu();
    }
    
}
