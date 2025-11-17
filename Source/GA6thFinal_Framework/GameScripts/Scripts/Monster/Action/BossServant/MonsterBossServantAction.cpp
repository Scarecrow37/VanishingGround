#include "pchScripts.h"
#include "MonsterBossServantAction.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "Stats/Enemy/EnemyStats.h"


namespace Monster
{
    namespace Action
    {
        void BossServantAction::OnActionExit() 
        {
            // 선언 시(턴 종료 시) 연격을 0으로 만든다.
            // (일단 미사용)
            //if (CharacterBase* owner = GetOwnerEnemy())
            //{
            //    CharacterStats* stats = owner->GetCharacterStats();
            //    stats->CurrentChainCount = 0;
            //}
        }
    } // namespace Action
} // namespace Monster