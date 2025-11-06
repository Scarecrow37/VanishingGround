#include "pchScripts.h"
#include "MonsterActionEnjoymentOfLoyalty.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

REGISTER_MONSTER_ACTION(Monster::Action::EnjoymentOfLoyalty)
namespace Monster
{
    namespace Action
    {
        EnjoymentOfLoyalty::EnjoymentOfLoyalty() : BossServantAction("", 2.0f) {}
        EnjoymentOfLoyalty::~EnjoymentOfLoyalty() = default;
        void EnjoymentOfLoyalty::OnActionEnter() 
        {
            Behavior();
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
            }
        }
    } // namespace Action
} // namespace Monster