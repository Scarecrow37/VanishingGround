#include "pchScripts.h"
#include "ApplyDamage.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "TurnSystem/TurnSystemHelper.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

REGISTER_TURN_ACTION(ApplyDamage)

REFLECT_FUNCTION(ApplyDamage)

ApplyDamage::ApplyDamage() 
{
    UpdateInfoText();
}
ApplyDamage::~ApplyDamage() {}

const std::string& ApplyDamage::GetActionName()
{
    static const std::string actionName = (const char*)u8"데미지 적용";
    return actionName;
}

const std::string& ApplyDamage::GetActionInfo()
{
    return _actionInfo;
}

void ApplyDamage::ImGuiDrawActionEditor() 
{
    using namespace u8_literals;
    ImGuiDrawPropertys();

    static ReflectHelper::ImGuiDraw::InputAutoSetting setting = [] 
    {
        ReflectHelper::ImGuiDraw::InputAutoSetting setting;
        setting._float.format = "%.1f";
        return setting;
    }();

    if (TurnSystemHelper::DrawTargetComboboxWithToolTip(ReflectFields->Target, true))
    {
        UpdateInfoText();
    }

    constexpr std::array<std::u8string_view, 2> TRIGGER_TOOLTIP = 
    { 
        GetTriggerToolTip(TriggerType::QTE_END),
        GetTriggerToolTip(TriggerType::WEAPON_KILL_ENEMY)
    };
    ImGuiHelper::EnumCombo<TriggerType>(u8"트리거"_c_str, ReflectFields->Trigger, [this](std::pair<std::string_view, TriggerType> pair)
    { 
        auto& [str, value]     = pair;
        ReflectFields->Trigger = value;
    },
    &TRIGGER_TOOLTIP);

    int damage = Damage;
    if (ImGui::DragInt(u8"데미지"_c_str, &damage))
    {
        Damage = damage;
        UpdateInfoText();
    }

    ImguiDrawConditionEditor();
}

void ApplyDamage::OnPlayerQTEResult(Player& player, const QTE::OverallResult& result) 
{
    if (TriggerType::QTE_END == ReflectFields->Trigger)
    {
        if (EvaluateConditions())
        {
            std::vector<CharacterBase*> targets = TurnSystemHelper::GetTargetCharacters(ReflectFields->Target);
            for (auto& target : targets)
            {
                target->TakeDamage(ReflectFields->Damage);
            }
        }
    }
}

void ApplyDamage::OnEnemyDeadByWeapon(Enemy& enemy, WeaponElement& weapon)
{
    if (TriggerType::WEAPON_KILL_ENEMY == ReflectFields->Trigger)
    {
        if (EvaluateConditions())
        {
            std::vector<CharacterBase*> targets = TurnSystemHelper::GetTargetCharacters(ReflectFields->Target);
            for (auto& target : targets)
            {
                target->TakeDamage(ReflectFields->Damage);
            }
        }
    }
}

void ApplyDamage::DeserializedReflectEvent() 
{
    UpdateInfoText();
}

void ApplyDamage::UpdateInfoText() 
{
     using namespace u8_literals;

    _actionInfo.clear();
    _actionInfo = (const char*)GetTriggerToolTip(ReflectFields->Trigger);
    _actionInfo += " ";
    _actionInfo += (const char*)TurnSystemHelper::GetTurnTargetToolTip(ReflectFields->Target).data();
    _actionInfo += u8"에게 "_c_str;
    _actionInfo += std::to_string(ReflectFields->Damage);
    _actionInfo += u8" 의 데미지"_c_str;
}
