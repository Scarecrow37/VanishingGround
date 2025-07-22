#include "pchScripts.h"
#include "ChainCondition.h"

#include <Token/TokenSystem.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnMode/TurnMode.h>

using namespace u8_literals;

ChainCondition::ChainCondition() 
{
    UpdateConditionInfo();
}

bool ChainCondition::Evaluate()
{
    bool result = false;
    if (TurnMode* turnMode = TurnMode::GetInstance())
    {
        CombatStartPhase* combatStartPhase = turnMode->States->CombatStartPhase;
        if (combatStartPhase)
        {
            static std::vector<CharacterBase*> targetList;
            targetList.clear();
            Target target = ReflectFields->Target;
            switch (target)
            {
            default:
                return false;
            case Target::SELF: {
                const auto& self = TurnMode::Battle::GetLastAttacker().lock();
                if (self)
                {
                    targetList.push_back(self.get());
                }
                break;
            }
            case Target::PLAYER: {
                Player* player = combatStartPhase->GetPlayer();
                if (player)
                {
                    targetList.push_back(player);
                }
                break;
            }
            case Target::ENEMY: {
                const auto& enemy = TurnMode::Battle::GetLastTargetEnemy().lock();
                if (enemy)
                {
                    targetList.push_back(enemy.get());
                }
                break;
            }
            case Target::ALL_ENEMIES: {
                auto& enemys = combatStartPhase->GetEnemies();
                for (auto& enemy : enemys)
                {
                    targetList.push_back(enemy);
                }
                break;
            }
            case Target::ALL: {
                auto& characters = combatStartPhase->GetCharacters();
                for (auto& character : characters)
                {
                    targetList.push_back(character);
                }
                break;
            }
            }

            if (false == targetList.empty())
            {
                Operator oper = ReflectFields->Operator;
                result      = true;
                int value1   = ReflectFields->Value1;
                int value2   = ReflectFields->Value2;
                for (auto& target : targetList)
                {
                    int targetChainCount = target->ChainCount;
                    switch (oper)
                    {
                    case ChainCondition::Operator::GREATER_EQUAL:
                        result &= targetChainCount >= value1;
                        break;
                    case ChainCondition::Operator::LESS_EQUAL:
                        result &= targetChainCount <= value1;
                        break;
                    case ChainCondition::Operator::EQUAL:
                        result &= targetChainCount == value1;
                        break;
                    case ChainCondition::Operator::BETWEEN:
                        result &= value1 <= targetChainCount && targetChainCount <= value2;
                        break;
                    case ChainCondition::Operator::MULTIPLE_OF:
                        result &= targetChainCount % value1 == 0;
                        break;
                    default:
                        return false;
                        break;
                    }

                    if (false == result)
                    {
                        return result;
                    }
                }
            }
        }
    }
    return result;
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
            isEdit |= ReflectHelper::ImGuiDraw::Private::InputAuto(field, setting);
            index++;
        });
        ImGui::EndTable();

        if (isEdit)
        {
            UpdateConditionInfo();
        }
    }
}

const std::string& ChainCondition::GetConditionInfo() const
{
    return _conditionInfo;
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
    Target           target = ReflectFields->Target;
    switch (target)
    {
    case ChainCondition::Target::SELF:
        who = u8"자신의"_c_str;
        break;
    case ChainCondition::Target::PLAYER:
        who = u8"플레이어의"_c_str;
        break;
    case ChainCondition::Target::ENEMY:
        who = u8"적의"_c_str;
        break;
    case ChainCondition::Target::ALL_ENEMIES:
        who = u8"모든 적의"_c_str;
        break;
    case ChainCondition::Target::ALL:
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
        operInfo += u8" 이상이면"_c_str;
        break;
    case ChainCondition::Operator::LESS_EQUAL:
        operInfo = std::to_string(ReflectFields->Value1);
        operInfo += u8" 이하면"_c_str;
        break;
    case ChainCondition::Operator::EQUAL:
        operInfo = std::to_string(ReflectFields->Value1);
        operInfo += u8" 이면"_c_str;
        break;
    case ChainCondition::Operator::BETWEEN:
        operInfo = std::to_string(ReflectFields->Value1);
        operInfo += "~";
        operInfo += std::to_string(ReflectFields->Value2);
        operInfo += u8" 사이면"_c_str;
        break;
    case ChainCondition::Operator::MULTIPLE_OF:
        operInfo = std::to_string(ReflectFields->Value1);
        operInfo += u8" 의 배수면"_c_str;
        break;
    default:
        operInfo = STR_NULL;
        break;
    }
    _conditionInfo = std::format("{}{}{}{}", who, u8" 연격이 "_c_str, operInfo);
}