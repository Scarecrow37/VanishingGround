#include "pchScripts.h"
#include "MonsterActionSmokeSlash.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

REGISTER_MONSTER_ACTION(Monster::Action::SmokeSlash)
namespace Monster
{
    namespace Action
    {
        SmokeSlash::SmokeSlash() : Base("Attack1") {}
        SmokeSlash::~SmokeSlash() = default;

        void SmokeSlash::OnActionEnter() {}

        void SmokeSlash::OnActionUpdate() {}

        void SmokeSlash::OnActionExit() {}

        void SmokeSlash::OnActionReset() {}

        void SmokeSlash::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
        {
            const std::string& label = context->GetLabel();
            if ("Attack" == label)
            {
                Attack();
            }
        }

        /*
        플레이어에게 S_Param(1)의 데미지로 피해를 준다.
        자신에게 T_Param(1)을 부여한다.
        */
        void SmokeSlash::Attack() 
        {
            ActionParam damage = GetActionParam(1);
            if (auto* owner = GetOwnerEnemy())
            {
                TokenParam      tokenParam     = GetTokenParam(1);
                TokenInventory& tokenInventory = owner->GetTokenInventory();
                tokenInventory.AddTokenStackFromID(tokenParam.TokenID, tokenParam.Count);
                if (ParticleComponent* particle = owner->GetParticleComponent())
                {
                    particle->PlayEffect("buff");
                }
            }
            ProcessBattle(damage.Param);
        }
    }
}
