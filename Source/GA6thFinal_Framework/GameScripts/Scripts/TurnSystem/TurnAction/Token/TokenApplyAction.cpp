#include "pchScripts.h"
#include "TokenApplyAction.h"
#include <RevelationSystem/RevelationSystem.h>
#include <TurnSystem/TurnAction/TurnActionFactory.h>
#include <TurnSystem/TurnSystemHelper.h>
#include <Token/TokenSystem.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Stats/Weapon/WeaponStats.h>
#include <Stats/Player/PlayerStats.h>
#include <Stats/Enemy/EnemyStats.h>

REGISTER_TURN_ACTION(TokenApplyAction)

TokenApplyAction::TokenApplyAction() 
{
    UpdateActionInfo();
}

const std::string& TokenApplyAction::GetActionInfo()
{
    return _actionInfo;
}

void TokenApplyAction::ImGuiDrawActionEditor() 
{
    ImGui::Text("Action");
    std::string_view prevValue = TokenSystem::GetTokenNameFromID(ReflectFields->TokenID);
    if (prevValue.empty())
    {
        prevValue = STR_NULL;
    }
    if (ImGui::BeginCombo("Token ID##A72AE710-2115-4E9C-BC03-9709C4100F04", prevValue.data()))
    {
        bool selectable = false;
        for (auto& token : TokenSystem::GetTokenInstances())
        {
            const char* name = token->GetTokenName();
            int         id   = token->GetTokenID();
            selectable = ReflectFields->TokenID == id ? true : false;
            if (ImGui::Selectable(name, selectable))
            {
                TokenID = id;
            }
        }
        ImGui::EndCombo();
    }
    ReflectHelper::ImGuiDraw::Private::InputAuto(TokenCount, UmCore->ImGuiDrawPropertysSetting);
    TurnSystemHelper::DrawTargetComboboxWithToolTip(ReflectFields->TokenTarget, true);
    ImGui::Separator();
    ImGui::Text("Conditions");
    ImguiDrawConditionEditor();
}

const std::string& TokenApplyAction::GetActionName()
{
    static const std::string name = (const char*)u8"공격시 대상에게 토큰 부여";
    return name;
}

void TokenApplyAction::DeserializedReflectEvent() 
{
    UpdateActionInfo();
}

void TokenApplyAction::OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats, Enemy& target, EnemyStats& targetStats)
{
    if (EvaluateConditions())
    {
        std::vector<CharacterBase*> targets =TurnSystemHelper::GetTargetCharacters(ReflectFields->TokenTarget);
        if (false == targets.empty())
        {
            for (auto& applyTarget : targets)
            {
                applyTarget->GetTokenInventory().AddTokenStackFromID(TokenID, ReflectFields->TokenCount);
                std::string msg(applyTarget->gameObject->ToString());
                msg += (const char*)u8"에게 ";
                msg += std::format("{}{}{}{}", TokenSystem::GetTokenNameFromID(ReflectFields->TokenID),  (const char*)u8"토큰 ", ReflectFields->TokenCount, (const char*)u8"개 부여");
                UmLogger.Message(LogLevel::LEVEL_TRACE, msg);
            }
        } 
    }
}

void TokenApplyAction::UpdateActionInfo() 
{
    std::string_view tokenName = TokenSystem::GetTokenNameFromID(ReflectFields->TokenID);
    if (true == tokenName.empty())
    {
        tokenName = STR_NULL;
    }
    _actionInfo = (const char*)u8"공격시 ";
    _actionInfo += std::format("{}{}{}{}{}{}", rfl::enum_to_string(ReflectFields->TokenTarget), (const char*)u8"에게 ", tokenName, (const char*)u8"토큰 ", ReflectFields->TokenCount, (const char*)u8"개 부여");
}