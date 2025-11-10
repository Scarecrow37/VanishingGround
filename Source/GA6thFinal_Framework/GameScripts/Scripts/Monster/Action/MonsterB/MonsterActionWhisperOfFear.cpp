#include "pchScripts.h"
#include "MonsterActionWhisperOfFear.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "Particle/ParticleComponent.h"

REGISTER_MONSTER_ACTION(Monster::Action::WhisperOfFear)
namespace Monster
{
    namespace Action
    {
        WhisperOfFear::WhisperOfFear() : Base("Attack1") {}
        WhisperOfFear::~WhisperOfFear() = default;
        void WhisperOfFear::OnActionEnter()
        {
            _attackCount = 0;
        }
        void WhisperOfFear::OnActionUpdate() {}
        void WhisperOfFear::OnActionExit() {}
        void WhisperOfFear::OnActionReset() {}
        void WhisperOfFear::OnNotifiedAnimationEvent(const Timeline::EventContext* context)
        {
            const std::string& label = context->GetLabel();
            if ("Attack" == label)
            {
                if (_attackCount < GetActionContext().AttackCount)
                {
                    Attack();
                    ++_attackCount;
                }
            }
        }

        /*
        플레이어에게 S_Param(1)의 데미지로 피해를 준다.
        공격 시 플레이어에게 T_Param(1)을 부여한다.
        자신에게 T_Param(2)를 부여한다.
        */
        void WhisperOfFear::Attack() 
        {
            if (auto target = GetTargetFromString("Player").lock())
            {
                TokenParam      tokenParam     = GetTokenParam(1);
                TokenInventory& tokenInventory = target->GetTokenInventory();
                tokenInventory.AddTokenStackFromID(tokenParam.TokenID, tokenParam.Count);
                if (ParticleComponent* particle = target->GetParticleComponent())
                {
                    particle->PlayEffect("debuff");
                }
            }
            if (auto* owner = GetOwnerEnemy())
            {
                TokenParam      tokenParam     = GetTokenParam(2);
                TokenInventory& tokenInventory = owner->GetTokenInventory();
                tokenInventory.AddTokenStackFromID(tokenParam.TokenID, tokenParam.Count);
                if (ParticleComponent* particle = owner->GetParticleComponent())
                {
                    particle->PlayEffect("buff");
                }
            }
            ActionParam damage = GetActionParam(1);
            ProcessBattle(damage.Param);
        }
    } // namespace Action
} // namespace Monster