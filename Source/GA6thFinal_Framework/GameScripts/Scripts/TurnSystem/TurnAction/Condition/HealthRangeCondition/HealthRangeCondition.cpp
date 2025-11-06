#include "pchScripts.h"
#include "HealthRangeCondition.h"

#include <Stats/CharacterStats.h>   
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnSystemHelper.h>

REGISTER_TURN_ACTION_CONDITION(HealthRangeCondition)

REFLECT_FUNCTION(HealthRangeCondition)

HealthRangeCondition::HealthRangeCondition() 
{
    UpdateConditionInfo();
}

bool HealthRangeCondition::Evaluate()
{
    std::vector<CharacterBase*> targetList;
    GetTargetList(targetList);

    if (targetList.empty())
    {
        return false;
    }

    Operator oper   = ReflectFields->Operator;
    Unit     unit   = ReflectFields->Unit;
    float    value1 = ReflectFields->value1;
    float    value2 = ReflectFields->value2;

    for (const auto& target : targetList)
    {
        if (target->IsDead())
        {
            return false;
        }

        CharacterStats* stats = target->GetCharacterStats();
        if (!stats)
        {
            continue; 
        }

        int hp = stats->CurrentHP;
        int v1 = (int)std::round(value1);
        int v2 = (int)std::round(value2);

        if (Unit::PERCENT == unit)
        {
            v1 = (int)std::round(stats->MaxHP * value1);
            if (oper == Operator::BETWEEN)
            {
                v2 = (int)std::round(stats->MaxHP * value2);
            }
        }

        if (false == CheckOperation(hp, v1, v2))
        {
            return false;
        }
    }
    return true;
}

void HealthRangeCondition::DrawImguiEditor() 
{
    if (ImGui::BeginTable("Health Range Condition##4AFFA20A-6CE9-45F4-A050-CC5DAF5B1446", 5,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        static ReflectHelper::ImGuiDraw::InputAutoSetting setting = []() {
            ReflectHelper::ImGuiDraw::InputAutoSetting setting;
            setting._float.format = "%.1f";
            setting.ShowName = false;
            return setting;
        }();

        bool isEdit = false;
        ImGui::TableSetupColumn("Target");
        ImGui::TableSetupColumn("Operator");
        ImGui::TableSetupColumn("Value 1");
        ImGui::TableSetupColumn("Value 2");
        ImGui::TableSetupColumn("Unit");
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        const auto& view  = rfl::to_view(*ReflectFields);
        int         index = 0;
        view.apply([&](const auto& field) {
            ImGui::TableSetColumnIndex(index);

            if constexpr ("Target" == field.name())
            {
                isEdit |= TurnSystemHelper::DrawTargetComboboxWithToolTip(*field.value());
            }
            else
            {
                isEdit |= ReflectHelper::ImGuiDraw::Private::InputAuto(field, setting);
            }
            index++;
        });
        ImGui::EndTable();

        if (isEdit)
        {
            if (Unit::FLAT == ReflectFields->Unit)
            {
                ReflectFields->value1 = std::round(ReflectFields->value1);
                ReflectFields->value2 = std::round(ReflectFields->value2);
            }
            UpdateConditionInfo();
        }
    }
}

void HealthRangeCondition::DeserializedReflectEvent() 
{
    UpdateConditionInfo();
}

const std::string& HealthRangeCondition::GetConditionInfo()
{
    return _conditionInfo;
}

void HealthRangeCondition::GetTargetList(std::vector<class CharacterBase*>& targetList)
{
    targetList = TurnSystemHelper::GetTargetCharacters(ReflectFields->Target);
}

void HealthRangeCondition::UpdateConditionInfo()
{
    using namespace u8_literals;
    std::string_view who    = STR_NULL;
    TurnTarget       target = ReflectFields->Target;
    switch (target)
    {
    case TurnTarget::SELF:
        who = u8"자신의"_c_str;
        break;
    case TurnTarget::PLAYER:
        who = u8"플레이어의"_c_str;
        break;
    case TurnTarget::ENEMY:
        who = u8"적의"_c_str;
        break;
    case TurnTarget::ALL_ENEMIES:
        who = u8"모든 적의"_c_str;
        break;
    case TurnTarget::ALL:
        who = u8"모든 캐릭터의"_c_str;
        break;
    default:
        break;
    }
    _conditionInfo = who;
    _conditionInfo += u8" 체력이 "_c_str;

    Operator oper = ReflectFields->Operator;
    std::string value1;
    std::string value2;
    Unit unit = ReflectFields->Unit;
    switch (unit)
    {
    case HealthRangeCondition::Unit::FLAT:
        ReflectFields->value1 = std::round(ReflectFields->value1);
        value1 = std::to_string((int)ReflectFields->value1);
        ReflectFields->value2 = std::round(ReflectFields->value2);
        value2 = std::to_string((int)ReflectFields->value2);
        break;
    case HealthRangeCondition::Unit::PERCENT:
        value1 = std::to_string(ReflectFields->value1 * 100.f);
        value1 += "%";
        value2 = std::to_string(ReflectFields->value2 * 100.f);
        value2 += "%";
        break;
    default:
        break;
    }

    switch (oper)
    {
    case HealthRangeCondition::Operator::GREATER_EQUAL:
        _conditionInfo += value1;
        _conditionInfo += u8" 이상"_c_str;
        break;
    case HealthRangeCondition::Operator::LESS_EQUAL:
        _conditionInfo += value1;
        _conditionInfo += u8" 이하"_c_str;
        break;
    case HealthRangeCondition::Operator::EQUAL:
        _conditionInfo += value1;
        break;
    case HealthRangeCondition::Operator::BETWEEN:
        _conditionInfo += value1;
        _conditionInfo += "~";
        _conditionInfo += value2;
        _conditionInfo += u8"사이"_c_str;
        break;
    default:
        break;
    }
}

bool HealthRangeCondition::CheckEvaluate(CharacterBase* character)
{
    if (character)
    {
        Operator oper   = ReflectFields->Operator;
        Unit     unit   = ReflectFields->Unit;
        float    value1 = ReflectFields->value1;
        float    value2 = ReflectFields->value2;
 
        if (character->IsDead())
        {
            return false;
        }

        if (CharacterStats* stats = character->GetCharacterStats())
        {
            int hp = stats->CurrentHP;
            int v1 = (int)std::round(ReflectFields->value1);
            int v2 = (int)std::round(ReflectFields->value2);
    
            if (Unit::PERCENT == unit)
            {
                v1 = (int)std::round(stats->MaxHP * value1);
                if (oper == Operator::BETWEEN)
                {
                    v2 = (int)std::round(stats->MaxHP * value2);
                }
            }
            return CheckOperation(hp, v1, v2);
        }
    }
    return false;
}

bool HealthRangeCondition::CheckOperation(int currentHP, int v1, int v2)
{
    switch (ReflectFields->Operator)
    {
    case Operator::GREATER_EQUAL:
        return currentHP >= v1;
    case Operator::LESS_EQUAL:
        return currentHP <= v1;
    case Operator::EQUAL:
        return currentHP == v1;
    case Operator::BETWEEN:
        return v1 <= currentHP && currentHP <= v2;
    default:
        return false;
    }
}
