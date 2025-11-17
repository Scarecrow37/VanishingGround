#include "pchScripts.h"
#include "Debugging.h"
#include "Map/MapManager.h"
#include "Map/Stage.h"

UMREAL_COMPONENT(Debugging)

void Debugging::Update() 
{
    Debugger()([this] {
        // 아래는 디버그용 코드입니다.
        if (MapManager* map = SingletonComponent<MapManager>::GetInstance())
        {
            ImGuiHelper::AlignedText("Map Manager", ImGuiHelper::LEFT, 0.8f);
            if (auto currStage = map->GetCurrentSelectedStage())
            {
                const std::string& name = currStage->gameObject->Name;
                ImGui::Text("Current Stage: ");
                ImGui::Separator();
                ImGui::Text(name.c_str());
            }
        }
    });
}