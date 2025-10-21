#include "pchScripts.h"
#include "MonsterActionBloodRitual.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

REGISTER_MONSTER_ACTION(Monster::Action::BloodRitual)
namespace Monster
{
    namespace Action
    {
        BloodRitual::BloodRitual() : Base("Attack1") {}
        BloodRitual::~BloodRitual() = default;

        void BloodRitual::OnActionEnter() {}

        void BloodRitual::OnActionUpdate() {}

        void BloodRitual::OnActionExit() {}

        void BloodRitual::OnActionReset() {}

        void BloodRitual::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
        {
            const std::string& label = context->GetLabel();
            if ("Behavior" == label)
            {
                Behavior();
            }
        }

        /*
        자신에게 T_Param(1)을 부여한다.
        */
        void BloodRitual::Behavior() 
        {
            if(auto* target = GetTarget())
            {
                TokenParam      tokenParam     = GetTokenParam(1);
                TokenInventory& tokenInventory = target->GetTokenInventory();
                tokenInventory.AddTokenStackFromID(tokenParam.TokenID, tokenParam.Count);
            }
        }
    } // namespace Action
} // namespace Monster