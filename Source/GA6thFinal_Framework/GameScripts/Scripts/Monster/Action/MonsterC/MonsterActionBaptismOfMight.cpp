#include "pchScripts.h"
#include "MonsterActionBaptismOfMight.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

REGISTER_MONSTER_ACTION(Monster::Action::BaptismOfMight)
namespace Monster
{
    namespace Action
    {
        BaptismOfMight::BaptismOfMight() : Base("Attack3") {}
        BaptismOfMight::~BaptismOfMight() = default;
        void BaptismOfMight::OnActionEnter() {}
        void BaptismOfMight::OnActionUpdate() {}
        void BaptismOfMight::OnActionExit() {}
        void BaptismOfMight::OnActionReset() {}
        void BaptismOfMight::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
        {
            const std::string& label = context->GetLabel();
            if ("Attack" == label)
            {
                Behavior();
            }
        }
        /*
        자신에게 T_Param(1)을 부여한다.
        */
        void BaptismOfMight::Behavior() 
        {
            if(auto* owner = GetOwnerEnemy())
            {
                TokenParam      tokenParam     = GetTokenParam(1);
                TokenInventory& tokenInventory = owner->GetTokenInventory();
                tokenInventory.AddTokenStackFromID(tokenParam.TokenID, tokenParam.Count);
            }
        }
    } // namespace Action
} // namespace Monster