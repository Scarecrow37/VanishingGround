#include "pchScripts.h"
#include "MonsterActionBlackSacrament.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

REGISTER_MONSTER_ACTION(Monster::Action::BlackSacrament)
namespace Monster
{
    namespace Action
    {
        BlackSacrament::BlackSacrament() : BossServantAction("Attack1") {}
        BlackSacrament::~BlackSacrament() = default;
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
        자신에게 T_Param(2)를 S_Param(1)~S_Param(2) 만큼 부여한다.
        */
        void BlackSacrament::Behavior() 
        {
            {   // 바른에게 T_Param(1)을 부여한다.
                TokenParam tokenParam = GetTokenParam(1);
                if (CharacterBase* target = GetTarget())
                {
                    TokenInventory& tokenInventory = target->GetTokenInventory();
                    tokenInventory.AddTokenStackFromID(tokenParam.TokenID, tokenParam.Count);
                }
            }
            {   // 자신에게 T_Param(2)를 S_Param(1)~S_Param(2) 만큼 부여한다.
                if (CharacterBase* target = GetOwnerEnemy())
                {
                    TokenParam  tokenParam     = GetTokenParam(2);
                    ActionParam actionParam1   = GetActionParam(1);
                    ActionParam actionParam2   = GetActionParam(2);

                    int randomRange = Random::Range(actionParam1.Param, actionParam2.Param);

                    TokenInventory& tokenInventory = target->GetTokenInventory();
                    tokenInventory.AddTokenStackFromID(tokenParam.TokenID, randomRange);
                }
            }
        }
    }
}
