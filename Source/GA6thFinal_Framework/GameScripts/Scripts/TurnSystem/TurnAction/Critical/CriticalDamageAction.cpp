#include "pchScripts.h"
#include "CriticalDamageAction.h"
#include <RevelationSystem/RevelationSystem.h>
#include <TurnSystem/TurnAction/Condition/TokenCondition/TokenCondition.h>

REGISTER_TURN_ACTION(CriticalDamageAction)

CriticalDamageAction::CriticalDamageAction() 
{
    UpdateActionInfo();
}

CriticalDamageAction::~CriticalDamageAction() 
{

}

const std::string& CriticalDamageAction::GetActionInfo()
{ 
    return _actionInfo;
}

void CriticalDamageAction::ImGuiDrawActionEditor() 
{
    static ReflectHelper::ImGuiDraw::InputAutoSetting setting = [] 
    {
        ReflectHelper::ImGuiDraw::InputAutoSetting setting;
        setting._float.format = "%.1f";
        return setting;
    }();

    ImGui::Begin("Action Editor##963EABCA-C1CE-414C-8B4C-9E9D3FFBD398");
    {
        ImGuiDrawPropertys(setting);
        ImguiDrawConditionEditor();
    }
    ImGui::End();
}

void CriticalDamageAction::DeserializedReflectEvent() 
{
    UpdateActionInfo();
}

void CriticalDamageAction::UpdateActionInfo() 
{
    _actionInfo = std::format("{}{}{}", (const char*)u8"치명타 피해 + ", ReflectFields->AdditionalDamage * 100.f, "%");
}


const std::string& CriticalDamageAction::GetActionName()
{
    static const std::string name = (const char*)u8"치명타 데미지 증가";
    return name;
}
