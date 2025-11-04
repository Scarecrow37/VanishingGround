#include "pchScripts.h"
#include "ChainCondition.h"

#include <Token/TokenSystem.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnSystemHelper.h>
#include <Stats/Enemy/EnemyStats.h>
#include <Stats/Player/PlayerStats.h>
#include <WeaponSystem/WeaponSystem.h>

REGISTER_TURN_ACTION_CONDITION(ChainCondition)

REFLECT_FUNCTION(ChainCondition)

using namespace u8_literals;

ChainCondition::ChainCondition() 
{
    UpdateConditionInfo();
}

bool ChainCondition::Evaluate()
{
    std::vector<CharacterBase*> targetList = TurnSystemHelper::GetTargetCharacters(ReflectFields->Target);
    if (targetList.empty())
    {
        return false;
    }

    for (const auto& target : targetList)
    {
        int targetChainCount = target->ChainCount;
        if (false == CheckEvaluate(targetChainCount))
        {
            return false;
        }
    }

    return true;
}

void ChainCondition::DrawImguiEditor() 
{
    if (ImGui::BeginTable("Chain Condition##72F2E242-5571-49B1-89F5-8B6F72188B51", 4,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        static ReflectHelper::ImGuiDraw::InputAutoSetting setting = []() {
            ReflectHelper::ImGuiDraw::InputAutoSetting setting;
            setting.ShowName = false;
            return setting;
        }();

        bool isEdit = false;
        ImGui::TableSetupColumn("Target");
        ImGui::TableSetupColumn("Operator");
        ImGui::TableSetupColumn("Value 1");
        ImGui::TableSetupColumn("Value 2");
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        const auto& view  = rfl::to_view(*ReflectFields);
        int         index = 0;
        view.apply([&](const auto& field) {
            ImGui::TableSetColumnIndex(index);
            if constexpr (field.name() == "Target")
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
            UpdateConditionInfo();
        }
    }
}

const std::string& ChainCondition::GetConditionInfo()
{
    return _conditionInfo;
}

bool ChainCondition::CheckEvaluate(int chainCount)
{
    Operator oper   = ReflectFields->Operator;
    int      value1 = ReflectFields->Value1;
    int      value2 = ReflectFields->Value2;

    switch (oper)
    {
    case Operator::GREATER_EQUAL:
        return chainCount >= value1;
    case Operator::LESS_EQUAL:
        return chainCount <= value1;
    case Operator::EQUAL:
        return chainCount == value1;
    case Operator::BETWEEN:
        return value1 <= chainCount && chainCount <= value2;
    case Operator::MULTIPLE_OF:
        return (0 < chainCount) && (chainCount % value1 == 0);
    default:
        return false;
    }
}

void ChainCondition::SerializedReflectEvent() 
{

}

void ChainCondition::DeserializedReflectEvent() 
{
    UpdateConditionInfo();
}

void ChainCondition::UpdateConditionInfo() 
{
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
    std::string operInfo;
    Operator    oper = ReflectFields->Operator;
    switch (oper)
    {
    case ChainCondition::Operator::GREATER_EQUAL:
        operInfo = std::to_string(ReflectFields->Value1);
        operInfo += u8" 이상"_c_str;
        break;
    case ChainCondition::Operator::LESS_EQUAL:
        operInfo = std::to_string(ReflectFields->Value1);
        operInfo += u8" 이하"_c_str;
        break;
    case ChainCondition::Operator::EQUAL:
        operInfo = std::to_string(ReflectFields->Value1);
        operInfo += u8" "_c_str;
        break;
    case ChainCondition::Operator::BETWEEN:
        operInfo = std::to_string(ReflectFields->Value1);
        operInfo += "~";
        operInfo += std::to_string(ReflectFields->Value2);
        operInfo += u8" 사이"_c_str;
        break;
    case ChainCondition::Operator::MULTIPLE_OF:
        operInfo = std::to_string(ReflectFields->Value1);
        operInfo += u8" 의 배수"_c_str;
        break;
    default:
        operInfo = STR_NULL;
        break;
    }
    _conditionInfo = std::format("{}{}{}", who, u8" 연격이 "_c_str, operInfo);
}

bool ChainCondition::CheckEvaluate(CharacterBase* character)
{
    if (WeaponSystem* system = SingletonComponent<WeaponSystem>::GetInstance())
    {
        if (character)
        {
            if (CharacterStats* stats = character->GetCharacterStats())
            {
                auto& element = system->GetCurrentWeaponElement();
                for (int count = stats->CurrentChainCount; count <= element.Stats.AttackCount; ++count)
                {
                    if (CheckEvaluate(count))
                    {
                        return true;
                    }
                }
                return false;
            }
        }
    }
    return false;
}
