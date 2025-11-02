#include "pchScripts.h"
#include "MonsterActionEnjoymentOfLoyalty.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

REGISTER_MONSTER_ACTION(Monster::Action::EnjoymentOfLoyalty)
namespace Monster
{
    namespace Action
    {
        EnjoymentOfLoyalty::EnjoymentOfLoyalty() : BossServantAction("Attack1") {}
        EnjoymentOfLoyalty::~EnjoymentOfLoyalty() = default;
        void EnjoymentOfLoyalty::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
        {
            const std::string& label = context->GetLabel();
            if ("Behavior" == label)
            {
                Behavior();
            }
        }
        /*
        바른의 T_Param(1), T_Param(2), T_Param(3), 중 하나를 제거한다.
        바른이 갖고 있는 토큰 중에서 하나를 선정해 시전한다.
        자신에게 T_Param(4)를 S_Param(1)~S_Param(2) 만큼 부여한다.
        */
        void EnjoymentOfLoyalty::Behavior() 
        {
            if (auto* owner = GetTarget())
            {
                TokenInventory& tokenInventory = owner->GetTokenInventory();
                size_t tokenParamCount = GetTokenParamCount();
                std::vector<int> availableTokenID;

                // i 는 1부터 시작
                for (size_t i = 1; i <= 3; ++i)
                {
                    TokenParam tokenParam = GetTokenParam(i);
                    if (tokenInventory.HasTokenFromID(tokenParam.TokenID))
                    {
                        availableTokenID.push_back(tokenParam.TokenID);
                    }
                }
                if (false == availableTokenID.empty())
                {
                    size_t randomIndex = Random::Index(availableTokenID.size());
                    tokenInventory.RemoveTokenStackFromID(availableTokenID[randomIndex]);
                }

                {   // 자신에게 T_Param(4)를 S_Param(1)~S_Param(2) 만큼 부여한다.
                    if (CharacterBase* target = GetOwnerEnemy())
                    {
                        TokenParam  tokenParam   = GetTokenParam(4);
                        ActionParam actionParam1 = GetActionParam(1);
                        ActionParam actionParam2 = GetActionParam(2);

                        int randomRange = Random::Range(actionParam1.Param, actionParam2.Param);

                        TokenInventory& tokenInventory = target->GetTokenInventory();
                        tokenInventory.AddTokenStackFromID(tokenParam.TokenID, randomRange);
                    }
                }
            }
        }
    } // namespace Action
} // namespace Monster