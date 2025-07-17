#include "pchScripts.h"
#include "CriticalDamageAction.h"
#include <RevelationSystem/RevelationSystem.h>

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
    constexpr auto conditions = rfl::get_enumerator_array<CriticalDamageCondition>();
    auto enumToStrig = rfl::enum_to_string(ReflectFields->Condition);
    if (ImGui::BeginCombo((const char*)u8"조건", enumToStrig.data()))
    {
        for (auto& [name, value] : conditions)
        {
            bool isSelected = ReflectFields->Condition == value;
            if (ImGui::Selectable(name.data(), isSelected))
            {
                ReflectFields->Condition = value;
            }
            switch (value)
            {
            case CriticalDamageCondition::ALWAYS:
                ImGuiHelper::HoveredToolTip(u8"항상 발동합니다.");
                break;
            case CriticalDamageCondition::TARGET_BLEED:
                ImGuiHelper::HoveredToolTip(u8"공격 대상이 출혈 상태일때 발동합니다.");
                break;
            default:
                break;
            }
        }
        ImGui::EndCombo();
    }

    static ReflectHelper::ImGuiDraw::InputAutoSetting setting = [] 
    {
        ReflectHelper::ImGuiDraw::InputAutoSetting setting;
        setting._float.format = "%.1f";
        return setting;
    }();
    ImGuiDrawPropertys(setting);
}

void CriticalDamageAction::DeserializedReflectEvent() 
{
    UpdateActionInfo();
}

void CriticalDamageAction::UpdateActionInfo() 
{
    std::u8string_view condition;
    switch (ReflectFields->Condition)
    {
    case CriticalDamageCondition::ALWAYS:
        condition = u8"";
        break;
    case CriticalDamageCondition::TARGET_BLEED:
        condition = u8"출혈 상대로 ";
        break;
    default:
        condition = u8"ERROR ";
        break;
    }
    _actionInfo = std::format("{}{}{}{}", (const char*)condition.data(), (const char*)u8"치명타 피해 + ",
                              ReflectFields->AdditionalDamage * 100.f, "%");
}

bool CriticalDamageAction::Evaluate(CriticalDamageCondition condition, CharacterBase* attacker, CharacterBase* target)
{
    switch (condition)
    {
    case CriticalDamageCondition::ALWAYS:
        return true;
    case CriticalDamageCondition::TARGET_BLEED:
        return false;
    default:
        return false;
    }
}

const std::string& CriticalDamageAction::GetActionName()
{
    static const std::string name = (const char*)u8"치명타 데미지 증가";
    return name;
}
