#include "pchScripts.h"
#include "TokenCondition.h"
#include <Token/TokenSystem.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnSystemHelper.h>

REGISTER_TURN_ACTION_CONDITION(TokenCondition)

REFLECT_FUNCTION(TokenCondition)

using namespace u8_literals;

TokenCondition::TokenCondition() 
{
    UpdateConditionInfo();
}

bool TokenCondition::Evaluate()
{
    std::vector<CharacterBase*> targetList;
    GetTargetList(targetList);

    if (targetList.empty())
    {
        return false;
    }

    Operator oper    = ReflectFields->Operator;
    int      tokenID = ReflectFields->TokenType;
    int      value   = ReflectFields->Value;

    auto CheckOperation = [&](int tokenCount) 
    {
        switch (oper)
        {
            case Operator::GREATER_EQUAL: return tokenCount >= value;
            case Operator::LESS_EQUAL:    return tokenCount <= value;
            case Operator::EQUAL:         return tokenCount == value;
            default:                      return false;
        }
    };

    for (const auto& target : targetList)
    {
        int targetTokenCount = target->GetTokenInventory().GetTokenStackFromID(tokenID);
        if (false == CheckOperation(targetTokenCount))
        {
            return false;
        }
    }

    return true;
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
   
}

void TokenCondition::DeserializedReflectEvent() 
{
    UpdateConditionInfo();
}

void TokenCondition::UpdateConditionInfo() 
{  
    std::string_view who;
    TurnTarget target = ReflectFields->Target;
    switch (target)
    {
    default:
        who = u8"NULL의 "_c_str;
        break;
    case TurnTarget::SELF:
        who = u8"자신의 "_c_str;
        break;
    case TurnTarget::PLAYER:
        who = u8"플레이어의 "_c_str;
        break;
    case TurnTarget::ENEMY:
        who = u8"적의 "_c_str;
        break;
    case TurnTarget::ALL_ENEMIES:
        who = u8"모든 적의 "_c_str;
        break;
    case TurnTarget::ALL:
        who = u8"모든 캐릭터의 "_c_str;
        break;
    }
    std::string_view tokenName = TokenSystem::TokenIDToName(ReflectFields->TokenType);
    std::string_view token     = STR_NULL;
    if (false == tokenName.empty())
    {
        token = tokenName;
    }
    int              value = ReflectFields->Value;
    Operator         oper  = ReflectFields->Operator;
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
        operName = (const char*)u8"";
        break;
    default:
        operName = STR_NULL;
        break;
    }

    _conditionInfo =
        std::format("{}{}{}{}{}{}", who, token, (const char*)u8"토큰이 ", value, (const char*)u8"개 ", operName);
}

void TokenCondition::GetTargetList(std::vector<class CharacterBase*>& targetList)
{
    targetList = TurnSystemHelper::GetTargetCharacters(ReflectFields->Target);
}