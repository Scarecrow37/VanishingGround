#include "pchScripts.h"
#include "MonsterActionSkullBreaker.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

REGISTER_MONSTER_ACTION(Monster::Action::SkullBreaker)
namespace Monster
{
    namespace Action
    {
        SkullBreaker::SkullBreaker() : Base("Attack3") {}
        SkullBreaker::~SkullBreaker() = default;
        void SkullBreaker::OnActionEnter() {}
        void SkullBreaker::OnActionUpdate() {}
        void SkullBreaker::OnActionExit() {}
        void SkullBreaker::OnActionReset() {}
        void SkullBreaker::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
        {
            const std::string& label = context->GetLabel();
            if ("Attack" == label)
            {
                Attack();
            }
        }
        /*
        플레이어에게 S_Param(1)의 데미지로 피해를 준다.
        공격 시 플레이어에게 T_Param(1)을 부여한다.
        */
        void SkullBreaker::Attack() 
        {
            if (auto* target = GetTarget())
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