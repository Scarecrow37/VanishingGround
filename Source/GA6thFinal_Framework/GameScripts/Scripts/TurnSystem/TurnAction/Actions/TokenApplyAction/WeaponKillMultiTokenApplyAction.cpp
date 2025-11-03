#include "pchScripts.h"
#include "WeaponKillMultiTokenApplyAction.h"
#include "Token/TokenSystem.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "TurnSystem/TurnSystemHelper.h"

REGISTER_TURN_ACTION(WeaponKillMultiTokenApplyAction)

const std::string& WeaponKillMultiTokenApplyAction::GetActionName()
{
    using namespace u8_literals;
    const static std::string name = u8"무기 공격으로 적 처치시 토큰 여러개 부여"_c_str;
    return name;
}

const std::string& WeaponKillMultiTokenApplyAction::GetActionInfo()
{
    TryTokenSystemInfoUpdate();
    return _actionInfo;
}

void WeaponKillMultiTokenApplyAction::ImGuiDrawActionEditor()
{
    ImGuiDrawPropertys();
}

void WeaponKillMultiTokenApplyAction::OnEnemyDeadByWeapon(Enemy& enemy, WeaponElement& weapon)
{
    if (EvaluateConditions())
    {
        std::vector<CharacterBase*> targets = TurnSystemHelper::GetTargetCharacters(Target);
        if (false == targets.empty())
        {
            for (auto& target : targets)
            {
                for (auto& [id, count] : ReflectFields->Tokens)
                {
                    target->GetTokenInventory().AddTokenStackFromID(id, count);
                }
            }
        }
    }
}

void WeaponKillMultiTokenApplyAction::UpdateActionInfo()
{
    using namespace u8_literals;
    _actionInfo.clear();
    _actionInfo = u8"적을 무기로 처치하면 "_c_str;
    _actionInfo += (const char*)TurnSystemHelper::GetTurnTargetToolTip(Target).data();
    _actionInfo += u8"에게 "_c_str;
    for (auto& [id, count] : ReflectFields->Tokens)
    {
        _actionInfo += TokenSystem::TokenIDToName(id);
        _actionInfo += u8"토큰을 "_c_str;
        _actionInfo += std::to_string(count);
        _actionInfo += u8"개, "_c_str;
    }
    _actionInfo += u8"부여"_c_str;
}
