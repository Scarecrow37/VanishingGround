#include "pchScripts.h"
#include "MonsterActionImpute.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

REGISTER_MONSTER_ACTION(Monster::Action::Impute)
namespace Monster
{
    namespace Action
    {
        Impute::Impute() : Base("Attack1") {}
        Impute::~Impute() = default;
        void Impute::OnActionEnter() {}
        void Impute::OnActionUpdate() {}
        void Impute::OnActionExit() {}
        void Impute::OnActionReset() {}
        void Impute::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
        {
            const std::string& label = context->GetLabel();
            if ("Attack" == label)
            {
                Attack();
            }
        }
        /*
        플레이어게 S_Param(1)의 데미지로 피해를 준다.
        공격 시 플레이어에게 T_Param(1)(2)을 부여한다.
        */
        void Impute::Attack() 
        {
            ActionParam damage = GetActionParam(1);
            ProcessBattle(damage.Param);
            if (auto* target = GetTarget())
            {
                TokenInventory& tokenInventory = target->GetTokenInventory();
                {
                    TokenParam tokenParam = GetTokenParam(1);
                    tokenInventory.AddTokenStackFromID(tokenParam.TokenID, tokenParam.Count);
                }
                {
                    TokenParam tokenParam = GetTokenParam(2);
                    tokenInventory.AddTokenStackFromID(tokenParam.TokenID, tokenParam.Count);
                }
            }
        }
    } // namespace Action
} // namespace Monster