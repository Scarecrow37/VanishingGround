#include "pchScripts.h"
#include "RevelationElement.h"
#include <RevelationSystem/RevelationSystem.h>

void RevelationElement::ImGuiDrawPropertysEvent()
{
    RevelationSystem* system = RevelationSystem::GetInstance();
    if (system)
    {
        const auto& constructorsMap = system->GetInstanceConstructors();
        if (constructorsMap.find(ReflectFields->Action) == constructorsMap.end())
        {
            ReflectFields->Action = STR_NULL;
        }
        SetImGuiTableIndex();
        if (ImGui::BeginCombo("Action", ReflectFields->Action.c_str()))
        {
            for (auto& [key, func] : constructorsMap)
            {
                if (ImGui::Selectable(key.c_str()))
                {
                    ReflectFields->Action = key;
                }
            }
            ImGui::EndCombo();
        }
    }
    _imguiDrawIndex = 0;
}
