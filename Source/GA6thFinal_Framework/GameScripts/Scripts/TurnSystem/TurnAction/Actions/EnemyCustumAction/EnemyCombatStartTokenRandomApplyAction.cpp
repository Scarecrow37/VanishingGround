#include "EnemyCombatStartTokenRandomApplyAction.h"
#include "pchScripts.h"

#include "Token/TokenSystem.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "TurnSystem/TurnSystemHelper.h"

REGISTER_TURN_ACTION(EnemyCombatStartTokenRandomApplyAction)

const std::string& EnemyCombatStartTokenRandomApplyAction::GetActionInfo()
{
    TryTokenSystemInfoUpdate();
    return _actionInfo;
}

void EnemyCombatStartTokenRandomApplyAction::ImGuiDrawActionEditor()
{
    ImGuiDrawPropertys();
}

const std::string& EnemyCombatStartTokenRandomApplyAction::GetActionName()
{
    static const std::string name = (const char*)u8"(몬스터 전용)전투 시작 시 자신에게 토큰 랜덤하게 부여";
    return name;
}

void EnemyCombatStartTokenRandomApplyAction::OnEnemyCombatStartPhase(Enemy& enemy)
{
    if (EvaluateConditions())
    {
        const int        tokenID       = ReflectFields->TokenID;
        const int        tokenCountMin = ReflectFields->TokenCountMin;
        const int        tokenCountMax = ReflectFields->TokenCountMax;

        if (TokenSystem* tokenSystem = SingletonComponent<TokenSystem>::GetInstance())
        {
            TokenInventory& tokenInventory = enemy.GetTokenInventory();
            const int       randomIndex    = Random::Range(tokenCountMin, tokenCountMax);
            tokenInventory.AddTokenStackFromID(TokenID, randomIndex);
        }
    }
}

void EnemyCombatStartTokenRandomApplyAction::UpdateActionInfo()
{
    const int tokenID       = ReflectFields->TokenID;
    const int tokenCountMin = ReflectFields->TokenCountMin;
    const int tokenCountMax = ReflectFields->TokenCountMax;
    const TurnTarget target = ReflectFields->TokenTarget;

    std::string_view tokenName = TokenSystem::TokenIDToName(tokenID);
    if (true == tokenName.empty())
    {
        tokenName = STR_NULL;
    }
    _actionInfo = (const char*)u8"전투 시작시 ";
    _actionInfo += std::format("{}{}{}{}{}{}{}", 
        (const char*)u8"자신에게 ",
        tokenName, 
        (const char*)u8"토큰 ", 
        tokenCountMin,
        " ~ ", 
        tokenCountMax,
        (const char*)u8"개 부여");
}