#include "pchScripts.h"
#include "MonsterActionCollapsingPillar.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

REGISTER_MONSTER_ACTION(Monster::Action::CollapsingPillar)
namespace Monster
{
    namespace Action
    {
        CollapsingPillar::CollapsingPillar() : Base("Attack1") {}
        CollapsingPillar::~CollapsingPillar() = default;
        void CollapsingPillar::OnActionEnter() {}
        void CollapsingPillar::OnActionUpdate() {}
        void CollapsingPillar::OnActionExit() {}
        void CollapsingPillar::OnActionReset() {}
        void CollapsingPillar::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
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
        void CollapsingPillar::Attack() 
        {
            ActionParam damage = GetActionParam(1);
            ProcessBattle(damage.Param);
            if (CharacterBase* target = GetTarget())
            {
                TokenParam      tokenParam     = GetTokenParam(1);
                TokenInventory& tokenInventory = target->GetTokenInventory();
                tokenInventory.AddTokenStackFromID(tokenParam.TokenID, tokenParam.Count);
            }
        }
    } // namespace Action
} // namespace Monster