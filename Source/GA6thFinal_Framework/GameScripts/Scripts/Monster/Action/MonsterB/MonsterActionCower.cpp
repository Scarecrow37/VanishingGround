#include "pchScripts.h"
#include "MonsterActionCower.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "Particle/ParticleComponent.h"

REGISTER_MONSTER_ACTION(Monster::Action::Cower)
namespace Monster
{
    namespace Action
    {
        Cower::Cower() : Base("", 2.0f) {}
        Cower::~Cower() =default;
        void Cower::OnActionEnter()
        {
            Behavior();
        }
        void Cower::OnActionUpdate() {}
        void Cower::OnActionExit() {}
        void Cower::OnActionReset() {}
        void Cower::OnNotifiedAnimationEvent(const Timeline::EventContext* context) {}
        
        /*
        자신에게 T_Param(1)을 부여한다.
        자신에게 T_Param(2)을 부여한다.
        */
        void Cower::Behavior()
        {
            {   // 대상: Self
                TokenParam tokenParam1 = GetTokenParam(1);
                TokenParam tokenParam2 = GetTokenParam(2);
                auto       weakTarget = GetTargetFromString("Self");
                if (auto target = weakTarget.lock())
                {
                    TokenInventory& tokenInventory = target->GetTokenInventory();
                    tokenInventory.AddTokenStackFromID(tokenParam1.TokenID, tokenParam1.Count);
                    tokenInventory.AddTokenStackFromID(tokenParam2.TokenID, tokenParam2.Count);
                    if (ParticleComponent* particle = target->GetParticleComponent())
                    {
                        particle->PlayEffect("buff");
                    }
                }
            }
        }
    }
} // namespace Monster
