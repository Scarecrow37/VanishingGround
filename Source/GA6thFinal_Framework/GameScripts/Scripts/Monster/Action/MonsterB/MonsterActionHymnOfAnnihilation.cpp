#include "pchScripts.h"
#include "MonsterActionHymnOfAnnihilation.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "TurnSystem/TurnActor/Character/Enemy/State/EnemyDeadState.h"
#include "Animation/AnimationComponent.h"
#include "Token/Object/ProphecyDoom/ProphecyDoomToken.h"
#include "Token/TokenSystem.h"
#include "ContentMath/ContentMath.h"

REGISTER_MONSTER_ACTION(Monster::Action::HymnOfAnnihilation)
namespace Monster
{
    namespace Action
    {
        HymnOfAnnihilation::HymnOfAnnihilation() : Base("Attack2") {}; // 죽는 애니메이션에서 멈춰야하므로 직접 애니메이션 수행
        HymnOfAnnihilation::~HymnOfAnnihilation() = default;
        void HymnOfAnnihilation::OnActionEnter() 
        {
        }
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
            if (Enemy* owner = GetOwnerEnemy())
            {
                // 사망 애니메이션 재생 X
                EnemyDeadState* deadState = owner->GetFSMStates().Dead;
                if (deadState)
                {
                    deadState->SetDontChangeAnimation(true);
                }

                ActionParam     damageParam     = GetActionParam(1);
                TokenInventory& tokenInventory  = owner->GetTokenInventory();

                const int   tokenID     = TokenObject::ProphecyDoom::ID;
                const int   tokenCount  = tokenInventory.GetTokenStackFromID(tokenID);
                int         damage      = damageParam.Param;

                // 종말 예언 토큰을 소지할 시 데미지 증가
                if (tokenCount > 0)
                {
                    if (TokenSystem* tokenSystem = SingletonComponent<TokenSystem>::GetInstance())
                    {
                        if (IToken* token = tokenSystem->GetTokenFromID(tokenID))
                        {
                            // 종말 예언 토큰의 갯수만큼 데미지 퍼센테이지 
                            int addDamagePercentage = token->GetTokenParam(0) * tokenCount;
                            damage += ContentMath::CeilPercentage(damage, addDamagePercentage);
                        }
                    }
                }
                ProcessBattle(damage);
                owner->TakeDamage(owner->HP, false);
            }
            SetActionEnd();
        }
    }
} // namespace Monster