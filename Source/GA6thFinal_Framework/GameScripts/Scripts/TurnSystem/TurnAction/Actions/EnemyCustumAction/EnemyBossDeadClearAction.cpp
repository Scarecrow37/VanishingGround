#include "pchScripts.h"
#include "EnemyBossDeadClearAction.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

REGISTER_TURN_ACTION(EnemyBossDeadClearAction)
REFLECT_FUNCTION(EnemyBossDeadClearAction)

void EnemyBossDeadClearAction::OnEnemyDead(Enemy& enemy)
{
    if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
    {
        const auto& enemies = turnMode->GetEnemies();
        for (auto& enemy : enemies)
        {
            enemy->TakeDamage(999, false);
        }
    }
}

const std::string& EnemyBossDeadClearAction::GetActionName()
{
    static const std::string actionName = (const char*)u8"(몬스터 전용)자신이 죽을 시 모든 적 전멸";
    return actionName;
}

const std::string& EnemyBossDeadClearAction::GetActionInfo()
{
    static const std::string actionInfo = (const char*)u8"자신이 죽을 시 모든 적 전멸";
    return actionInfo;
}

void EnemyBossDeadClearAction::ImGuiDrawActionEditor() 
{
}
