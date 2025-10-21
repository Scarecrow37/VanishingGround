#include "pchScripts.h"
#include "MonsterActionRebinding.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

REGISTER_MONSTER_ACTION(Monster::Action::Rebinding)
namespace Monster
{
    namespace Action
    {
        Rebinding::Rebinding() : Base("Attack3") {}
        Rebinding::~Rebinding() = default;
        void Rebinding::OnActionEnter() {}
        void Rebinding::OnActionUpdate() {}
        void Rebinding::OnActionExit() {}
        void Rebinding::OnActionReset() {}
        void Rebinding::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
        {
            const std::string& label = context->GetLabel();
            if ("Behavior" == label)
            {
                Behavior();
            }
        }

        /*
        자신에게 T_Param(1)(2)을 부여한다.
        */
        void Rebinding::Behavior() 
        {
            if (auto* owner = GetOwnerEnemy())
            {
                TokenParam      tokenParam1    = GetTokenParam(1);
                TokenParam      tokenParam2    = GetTokenParam(2);
                TokenInventory& tokenInventory = owner->GetTokenInventory();
                tokenInventory.AddTokenStackFromID(tokenParam1.TokenID, tokenParam1.Count);
                tokenInventory.AddTokenStackFromID(tokenParam2.TokenID, tokenParam2.Count);
            }
        }
    } // namespace Action
} // namespace Monster