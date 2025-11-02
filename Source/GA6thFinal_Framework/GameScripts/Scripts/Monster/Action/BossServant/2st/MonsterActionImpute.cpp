#include "pchScripts.h"
#include "MonsterActionImpute.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

REGISTER_MONSTER_ACTION(Monster::Action::Impute)
namespace Monster
{
    namespace Action
    {
        Impute::Impute() : BossServantAction("Attack1") {}
        Impute::~Impute() = default;
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
        자신에게 T_Param(3)를 S_Param(2)~S_Param(3) 만큼 부여한다.
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

             {  // 자신에게 T_Param(3)를 S_Param(2)~S_Param(3) 만큼 부여한다.
                if (CharacterBase* target = GetOwnerEnemy())
                {
                    TokenParam  tokenParam   = GetTokenParam(3);
                    ActionParam actionParam1 = GetActionParam(2);
                    ActionParam actionParam2 = GetActionParam(3);

                    int randomRange = Random::Range(actionParam1.Param, actionParam2.Param);

                    TokenInventory& tokenInventory = target->GetTokenInventory();
                    tokenInventory.AddTokenStackFromID(tokenParam.TokenID, randomRange);
                }
            }
        }
    } // namespace Action
} // namespace Monster