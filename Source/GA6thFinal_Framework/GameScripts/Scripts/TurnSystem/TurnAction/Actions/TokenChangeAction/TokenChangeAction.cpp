#include "pchScripts.h"
#include "TokenChangeAction.h"
#include "TurnSystem/TurnSystemHelper.h"
#include "Token/TokenSystem.h"

REGISTER_TURN_ACTION(TokenChangeAction)

TokenChangeAction::TokenChangeAction() 
{
    UpdateActionInfo();
}

TokenChangeAction::~TokenChangeAction() = default;

const std::string& TokenChangeAction::GetActionName()
{
    const static std::string actionName = (const char*)u8"부여되는 토큰 변경";
    return actionName;
}

const std::string& TokenChangeAction::GetActionInfo()
{
    return _actionInfo;
}

void TokenChangeAction::ImGuiDrawActionEditor() 
{
    using namespace u8_literals;

    if (TurnSystemHelper::DrawTargetComboboxWithToolTip(ReflectFields->Target, true))
    {
        UpdateActionInfo();
    }

    std::string_view prevName = TokenSystem::GetTokenNameFromID(ReflectFields->TokenID);
    if (ImGui::BeginCombo(u8"변경할 토큰"_c_str, prevName.data()))
    {
        for (auto& token : TokenSystem::GetTokenInstances())
        {
            if (token)
            {
                int id = token->GetTokenID();
                const std::string& name = token->GetTokenName();
                if (false == name.empty())
                {
                    if (ImGui::Selectable(name.data(), name == prevName))
                    {
                        ReflectFields->TokenID = id;
                        UpdateActionInfo();
                    }
                }
            }
        }
        ImGui::EndCombo();
    }
    ImguiDrawConditionEditor();
}

void TokenChangeAction::DeserializedReflectEvent() 
{
    UpdateActionInfo();
}

void TokenChangeAction::OnTokenAddedStart(CharacterBase& target, int& tokenID, int& tokenCount) 
{
     using namespace u8_literals;
    if (EvaluateConditions())
    {
        std::string msg = TokenSystem::GetTokenNameFromID(tokenID);
        msg += u8" 토큰을 "_c_str;
        msg += TokenSystem::GetTokenNameFromID(ReflectFields->TokenID);
        msg += u8" 토큰으로 변경 "_c_str;
        UmLogger.Log(LogLevel::LEVEL_TRACE, msg);

        tokenID = ReflectFields->TokenID;
    }
}

void TokenChangeAction::UpdateActionInfo()
{
     using namespace u8_literals;
    _actionInfo.clear();
    std::u8string_view targetToolTip = TurnSystemHelper::GetTurnTargetToolTip(ReflectFields->Target);
    _actionInfo = (const char*)targetToolTip.data();
    _actionInfo += u8"의 부여되는 토큰을 "_c_str;
    _actionInfo += TokenSystem::GetTokenNameFromID(ReflectFields->TokenID);
    _actionInfo += u8" 토큰으로 변경합니다."_c_str;
}