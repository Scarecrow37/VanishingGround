#include "pchScripts.h"
#include "MonsterActionSmashStrike.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

REGISTER_MONSTER_ACTION(Monster::Action::SmashStrike)
namespace Monster
{
    namespace Action
    {
        SmashStrike::SmashStrike() : Base("Attack1") {}
        
        SmashStrike::~SmashStrike() = default;

        void SmashStrike::OnActionEnter() {}

        void SmashStrike::OnActionUpdate() {}

        void SmashStrike::OnActionExit() {}

        void SmashStrike::OnActionReset() {}

        void SmashStrike::OnNotifiedAnimationEvent(const Timeline::EventContext* context)
        {
            const std::string& label = context->GetLabel();
            if ("Attack" == label)
            {
                Attack();
            }
        }

        /*
        플레이어게 S_Param(1)의 데미지로 피해를 준다.
        공격 시 플레이어에게 T_Param(1)을 부여한다.
        */
        void SmashStrike::Attack() 
        {
            if (CharacterBase* target = GetTarget())
            {
                TokenParam      tokenParam     = GetTokenParam(1);
                TokenInventory& tokenInventory = target->GetTokenInventory();
                tokenInventory.AddTokenStackFromID(tokenParam.TokenID, tokenParam.Count);
            }
            ActionParam damage = GetActionParam(1);
            ProcessBattle(damage.Param);
        }
    } // namespace Action
} // namespace Monster