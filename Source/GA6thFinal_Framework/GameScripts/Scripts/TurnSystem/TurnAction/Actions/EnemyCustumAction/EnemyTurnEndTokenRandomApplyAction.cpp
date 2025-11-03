#include "pchScripts.h"
#include "EnemyTurnEndTokenRandomApplyAction.h"

#include "Token/TokenSystem.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "TurnSystem/TurnSystemHelper.h"

REGISTER_TURN_ACTION(EnemyTurnEndTokenRandomApplyAction)
REGISTER_TURN_ACTION(EnemyTurnEndTokenRandomSetAction)

const std::string& EnemyTurnEndTokenRandomApplyAction::GetActionInfo()
{
    TryTokenSystemInfoUpdate();
    return _actionInfo;
}

void EnemyTurnEndTokenRandomApplyAction::ImGuiDrawActionEditor()
{
    ImGuiDrawPropertys();
}

const std::string& EnemyTurnEndTokenRandomApplyAction::GetActionName()
{
    static const std::string name = (const char*)u8"(몬스터 전용)턴 종료 시 자신에게 토큰 랜덤하게 부여";
    return name;
}

void EnemyTurnEndTokenRandomApplyAction::OnEnemyTurnEnd(Enemy& enemy)
{
    if (EvaluateConditions())
    {
        const int tokenID       = ReflectFields->TokenID;
        const int tokenCountMin = ReflectFields->TokenCountMin;
        const int tokenCountMax = ReflectFields->TokenCountMax;

        if (TokenSystem* tokenSystem = SingletonComponent<TokenSystem>::GetInstance())
        {
            TokenInventory& tokenInventory = enemy.GetTokenInventory();
            const int       randomIndex    = Random::Range(tokenCountMin, tokenCountMax);
            tokenInventory.AddTokenStackFromID(TokenID, randomIndex);
        }
    }
}

void EnemyTurnEndTokenRandomApplyAction::UpdateActionInfo()
{
    const int        tokenID       = ReflectFields->TokenID;
    const int        tokenCountMin = ReflectFields->TokenCountMin;
    const int        tokenCountMax = ReflectFields->TokenCountMax;

    std::string_view tokenName = TokenSystem::TokenIDToName(tokenID);
    if (true == tokenName.empty())
    {
        tokenName = STR_NULL;
    }
    _actionInfo = (const char*)u8"턴 종료 시 ";
    _actionInfo += std::format("{}{}{}{}{}{}{}", (const char*)u8"자신에게 ", tokenName, (const char*)u8"토큰 ",
                               tokenCountMin, " ~ ", tokenCountMax, (const char*)u8"개 부여");
}


const std::string& EnemyTurnEndTokenRandomSetAction::GetActionInfo()
{
    TryTokenSystemInfoUpdate();
    return _actionInfo;
}

void EnemyTurnEndTokenRandomSetAction::ImGuiDrawActionEditor()
{
    ImGuiDrawPropertys();
}

const std::string& EnemyTurnEndTokenRandomSetAction::GetActionName()
{
    static const std::string name = (const char*)u8"(몬스터 전용)턴 종료 시 자신에게 토큰 랜덤하게 설정";
    return name;
}

void EnemyTurnEndTokenRandomSetAction::OnEnemyTurnEnd(Enemy& enemy)
{
    if (EvaluateConditions())
    {
        const int tokenID       = ReflectFields->TokenID;
        const int tokenCountMin = ReflectFields->TokenCountMin;
        const int tokenCountMax = ReflectFields->TokenCountMax;

        if (TokenSystem* tokenSystem = SingletonComponent<TokenSystem>::GetInstance())
        {
            TokenInventory& tokenInventory = enemy.GetTokenInventory();
            const int       randomIndex    = Random::Range(tokenCountMin, tokenCountMax);
            tokenInventory.SetTokenStackFromID(TokenID, randomIndex);
        }
    }
}

void EnemyTurnEndTokenRandomSetAction::UpdateActionInfo()
{
    const int        tokenID       = ReflectFields->TokenID;
    const int        tokenCountMin = ReflectFields->TokenCountMin;
    const int        tokenCountMax = ReflectFields->TokenCountMax;
    const TurnTarget target        = ReflectFields->TokenTarget;

    std::string_view tokenName = TokenSystem::TokenIDToName(tokenID);
    if (true == tokenName.empty())
    {
        tokenName = STR_NULL;
    }
    _actionInfo = (const char*)u8"턴 종료 시 ";
    _actionInfo += std::format("{}{}{}{}{}{}{}", (const char*)u8"자신에게 ", tokenName, (const char*)u8"토큰 ",
                               tokenCountMin, " ~ ", tokenCountMax, (const char*)u8"개 설정");
}