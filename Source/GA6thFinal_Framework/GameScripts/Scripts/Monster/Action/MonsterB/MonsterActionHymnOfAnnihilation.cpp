#include "pchScripts.h"
#include "MonsterActionHymnOfAnnihilation.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "TurnSystem/TurnActor/Character/Enemy/State/EnemyDeadState.h"

REGISTER_MONSTER_ACTION(Monster::Action::HymnOfAnnihilation)
namespace Monster
{
    namespace Action
    {
        HymnOfAnnihilation::HymnOfAnnihilation() : Base("Attack1") {}
        HymnOfAnnihilation::~HymnOfAnnihilation() = default;
        void HymnOfAnnihilation::OnActionEnter() {}
        void HymnOfAnnihilation::OnActionUpdate() {}
        void HymnOfAnnihilation::OnActionExit() {}
        void HymnOfAnnihilation::OnActionReset() {}
        void HymnOfAnnihilation::OnNotifiedAnimationEvent(const Timeline::EventContext* context)
        {
            const std::string& label = context->GetLabel();
            if ("Attack" == label)
            {
                Attack();
            }
        }

        /*
        플레이어게 S_Param(1)의 데미지로 피해를 준다.
        자신을 처치한다.
        */
        void HymnOfAnnihilation::Attack()
        {
            ActionParam damage = GetActionParam(1);
            ProcessBattle(damage.Param);
            if (Enemy* owner = GetOwnerEnemy())
            {
                owner->TakeDamage(owner->HP, false);
                EnemyDeadState* deadState = owner->GetFSMStates().Dead;
                if (deadState)
                {
                    deadState->SetDontChangeAnimation(true);
                }
            }
            SetActionEnd();
        }
    }
} // namespace Monster