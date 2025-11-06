#include "pchScripts.h"
#include "MonsterActionHymnOfAnnihilation.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "TurnSystem/TurnActor/Character/Enemy/State/EnemyDeadState.h"
#include "Animation/AnimationComponent.h"
#include "Token/Object/ProphecyDoom/ProphecyDoomToken.h"
#include "Token/TokenSystem.h"

REGISTER_MONSTER_ACTION(Monster::Action::HymnOfAnnihilation)
namespace Monster
{
    namespace Action
    {
        HymnOfAnnihilation::HymnOfAnnihilation() : Base("Attack2") {}; // 죽는 애니메이션에서 멈춰야하므로 직접 애니메이션 수행
        HymnOfAnnihilation::~HymnOfAnnihilation() = default;
        void HymnOfAnnihilation::OnActionEnter() 
        {
            if (AnimationComponent* animator = GetAnimationComponent())
            {
                auto weakOwner = GetWeakOwner();
                animator->SetCurrentAnimationEndCallback([weakOwner, this]() {
                    if (auto owner = weakOwner.lock())
                    {
                        this->SetActionEnd();
                        owner->gameObject->SetActive(false);
                    }
                });
            }
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
            const int tokenID = TokenObject::ProphecyDoom::ID;
            if (Enemy* owner = GetOwnerEnemy())
            {
                // 사망 애니메이션 재생 X
                EnemyDeadState* deadState = owner->GetFSMStates().Dead;
                if (deadState)
                {
                    deadState->SetDontChangeAnimation(true);
                }

                ActionParam damage = GetActionParam(1);
                TokenInventory& tokenInventory = owner->GetTokenInventory();
                // 종말 예언 토큰을 소지할 시 데미지 증가
                if (tokenInventory.HasTokenFromID(tokenID))
                {
                    if (TokenSystem* tokenSystem = SingletonComponent<TokenSystem>::GetInstance())
                    {
                        if (IToken* token = tokenSystem->GetTokenFromID(tokenID))
                        {
                            int param = token->GetTokenParam(0);
                            float factor = static_cast<float>(param) / 100.0f;
                            damage.Param = static_cast<int>(static_cast<float>(damage.Param) * (1.0f + factor));
                        }
                    }
                }
                ProcessBattle(damage.Param);
                owner->TakeDamage(owner->HP, false);
            }
            SetActionEnd();
        }
    }
} // namespace Monster