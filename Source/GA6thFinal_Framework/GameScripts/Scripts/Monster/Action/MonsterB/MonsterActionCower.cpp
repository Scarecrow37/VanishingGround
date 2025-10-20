#include "pchScripts.h"
#include "MonsterActionCower.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

REGISTER_MONSTER_ACTION(Monster::Action::Cower)
namespace Monster
{
    namespace Action
    {
        void Cower::OnActionEnter() 
        {
            if (ProcessAnimation("Attack0"))
            {
                SetActionEnd();
            }
        }
        void Cower::OnActionUpdate() {}
        void Cower::OnActionExit() {}
        void Cower::OnActionReset() {}
        void Cower::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
        {
            const std::string& label = context->GetLabel();
            if ("Attack" == label)
            {
                Behavior();
            }
        }
        
        /*
        자신에게 T_Param(1)을 부여한다.
        플레이어에게 T_Param(2)을 부여한다.
        */
        void Cower::Behavior()
        {
            {   // 대상: Self
                TokenParam tokenParam = GetTokenParam(1);
                auto       weakTarget = GetTargetFromString("Self");
                if (auto target = weakTarget.lock())
                {
                    TokenInventory& tokenInventory = target->GetTokenInventory();
                    tokenInventory.AddTokenStackFromID(tokenParam.TokenID, tokenParam.Count);
                }
            }
            {   // 대상: Player
                TokenParam tokenParam = GetTokenParam(2);
                auto       weakTarget = GetTargetFromString("Player");
                if (auto target = weakTarget.lock())
                {
                    TokenInventory& tokenInventory = target->GetTokenInventory();
                    tokenInventory.AddTokenStackFromID(tokenParam.TokenID, tokenParam.Count);
                }
            }
        }
    }
} // namespace Monster
