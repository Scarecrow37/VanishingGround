#include "pchScripts.h"
#include "TokenCondition.h"
#include <Token/TokenSystem.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

REGISTER_TURN_ACTION_CONDITION(TokenCondition)
using namespace u8_literals;

TokenCondition::TokenCondition() 
{
    UpdateConditionInfo();
}

bool TokenCondition::Evaluate()
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
            case TokenCondition::Target::SELF:
                {
                    const auto& self = TurnMode::Battle::GetLastAttacker().lock();
                    if (self)
                    {
                        targetList.push_back(self.get());
                    }
                    break;
                }
            case TokenCondition::Target::PLAYER:
                {
                    Player* player = combatStartPhase->GetPlayer();
                    if (player)
                    {
                        targetList.push_back(player);
                    }
                    break;
                }
            case TokenCondition::Target::ENEMY:
                {
                    const auto& enemy = TurnMode::Battle::GetLastTargetEnemy().lock();
                    if (enemy)
                    {
                        targetList.push_back(enemy.get());
                    }
                    break;
                }
            case TokenCondition::Target::ALL_ENEMIES:
                {
                    auto& enemys = combatStartPhase->GetEnemies();
                    for (auto& enemy : enemys)
                    {
                        targetList.push_back(enemy);
                    }
                    break;
                }
            case TokenCondition::Target::ALL:
                {
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
                result = true;
                int tokenID = ReflectFields->TokenType;
                int value   = ReflectFields->Value;
                for (auto& target : targetList)
                {                  
                    int targetTokenCount = target->GetTokenInventory().GetTokenStackFromID(tokenID);
                    switch (oper)
                    {
                    default:
                        return false;
                        break;
                    case TokenCondition::Operator::GREATER_EQUAL:
                        result &= targetTokenCount >= value;
                        break;
                    case TokenCondition::Operator::LESS_EQUAL:
                        result &= targetTokenCount <= value;
                        break;
                    case TokenCondition::Operator::EQUAL:
                        result &= targetTokenCount == value;
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

void TokenCondition::DrawImguiEditor() 
{
    if (ImGui::BeginTable("Token Condition##2796DA0B-FCA1-4074-9420-6F9D289C256B", 4,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        static ReflectHelper::ImGuiDraw::InputAutoSetting setting = []() 
        {
            ReflectHelper::ImGuiDraw::InputAutoSetting setting;
            setting.ShowName = false;
            return setting;
        }();

        bool isEdit = false;
        ImGui::TableSetupColumn("Target");
        ImGui::TableSetupColumn("Token Type");
        ImGui::TableSetupColumn("Operator");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        const auto& view = rfl::to_view(*ReflectFields);
        int index = 0;
        view.apply([&](const auto& field) 
        {
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

const std::string& TokenCondition::GetConditionInfo() const
{
    return _conditionInfo;
}

void TokenCondition::SerializedReflectEvent() 
{
    UpdateConditionInfo();
}

void TokenCondition::DeserializedReflectEvent() 
{
    UpdateConditionInfo();
}

void TokenCondition::UpdateConditionInfo() 
{  
    std::string_view who;
    Target target = ReflectFields->Target;
    switch (target)
    {
    default:
        who = u8"NULL의 "_c_str;
        break;
    case TokenCondition::Target::SELF:
        who = u8"자신의 "_c_str;
        break;
    case TokenCondition::Target::PLAYER:
        who = u8"플레이어의 "_c_str;
        break;
    case TokenCondition::Target::ENEMY:
        who = u8"공격 대상의 "_c_str;
        break;
    case TokenCondition::Target::ALL_ENEMIES:
        who = u8"모든 적의 "_c_str;
        break;
    case TokenCondition::Target::ALL:
        who = u8"모든 캐릭터의 "_c_str;
        break;
    }

    const std::string& tokenName = TokenSystem::GetTokenNameFromID(ReflectFields->TokenType);
    std::string_view   token     = STR_NULL;
    if (false == tokenName.empty())
    {
        token = tokenName;
    }
    int value = ReflectFields->Value;
    Operator oper = ReflectFields->Operator;
    std::string_view operName;
    switch (oper)
    {
    case TokenCondition::Operator::GREATER_EQUAL:
        operName = (const char*)u8"이상";
        break;
    case TokenCondition::Operator::LESS_EQUAL:
        operName = (const char*)u8"이하";
        break;
    case TokenCondition::Operator::EQUAL:
        operName = (const char*)u8"이면";
        break;
    default:
        operName = STR_NULL;
        break;
    }

    _conditionInfo = std::format("{}{}{}{}{}{}", who, token, (const char*)u8"토큰이 ", value, (const char*)u8"개 ", operName);
}
