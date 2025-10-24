#include "pchScripts.h"
#include "WeaponKillTokenApplyAction.h"
#include "TurnSystem/TurnSystemHelper.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "Token/TokenSystem.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"

REGISTER_TURN_ACTION(WeaponKillTokenApplyAction)

const std::string& WeaponKillTokenApplyAction::GetActionName()
{
    using namespace u8_literals;
    const static std::string name = u8"무기 공격으로 적 처치시 토큰 부여"_c_str;
    return name;
}

const std::string& WeaponKillTokenApplyAction::GetActionInfo()
{
    TryTokenSystemInfoUpdate();
    return _actionInfo;
}

void WeaponKillTokenApplyAction::ImGuiDrawActionEditor() 
{
    ImGuiDrawPropertys();
}

void WeaponKillTokenApplyAction::OnEnemyDeadByWeapon(Enemy& enemy, WeaponElement& weapon) 
{
    if (EvaluateConditions())
    {
        std::vector<CharacterBase*> targets = TurnSystemHelper::GetTargetCharacters(Target);
        if (false == targets.empty())
        {
            for (auto& target : targets)
            {
                target->GetTokenInventory().AddTokenStackFromID(TokenID, TokenCount);
            }
        }      
    }
}

void WeaponKillTokenApplyAction::UpdateActionInfo() 
{
    using namespace u8_literals;

    _actionInfo.clear();
    _actionInfo = u8"적을 무기로 처치하면 "_c_str;
    _actionInfo += (const char*)TurnSystemHelper::GetTurnTargetToolTip(Target).data();
    _actionInfo += u8"에게 "_c_str;
    _actionInfo += TokenSystem::TokenIDToName(TokenID);
    _actionInfo += u8"토큰을 "_c_str;
    _actionInfo += std::to_string(TokenCount);
    _actionInfo += u8"개 부여"_c_str;
}
