#include "pchScripts.h"
#include "MonsterActionBlackSacrament.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

REGISTER_MONSTER_ACTION(Monster::Action::BlackSacrament)
namespace Monster
{
    namespace Action
    {
        BlackSacrament::BlackSacrament() : Base("Attack1") {}

        BlackSacrament::~BlackSacrament() = default;

        void BlackSacrament::OnActionEnter() {}

        void BlackSacrament::OnActionUpdate() {}

        void BlackSacrament::OnActionExit() {}

        void BlackSacrament::OnActionReset() {}

        void BlackSacrament::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
        {
            const std::string& label = context->GetLabel();
            if ("Behavior" == label)
            {
                Behavior();
            }
        }

        /*
        바른에게 T_Param(1)을 부여한다.
        */
        void BlackSacrament::Behavior() 
        {
            TokenParam tokenParam = GetTokenParam(1);
            if (CharacterBase* target = GetTarget())
            {
                TokenInventory& tokenInventory = target->GetTokenInventory();
                tokenInventory.AddTokenStackFromID(tokenParam.TokenID, tokenParam.Count);
            }
        }
    }
}
