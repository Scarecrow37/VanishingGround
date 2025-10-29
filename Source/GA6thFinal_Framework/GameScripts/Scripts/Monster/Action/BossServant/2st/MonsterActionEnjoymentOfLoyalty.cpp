#include "pchScripts.h"
#include "MonsterActionEnjoymentOfLoyalty.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

REGISTER_MONSTER_ACTION(Monster::Action::EnjoymentOfLoyalty)
namespace Monster
{
    namespace Action
    {
        EnjoymentOfLoyalty::EnjoymentOfLoyalty() : Base("Attack1") {}
        EnjoymentOfLoyalty::~EnjoymentOfLoyalty() = default;
        void EnjoymentOfLoyalty::OnActionEnter() {}
        void EnjoymentOfLoyalty::OnActionUpdate() {}
        void EnjoymentOfLoyalty::OnActionExit() {}
        void EnjoymentOfLoyalty::OnActionReset() {}
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
        */
        void EnjoymentOfLoyalty::Behavior() 
        {
            if (auto* owner = GetTarget())
            {
                TokenInventory& tokenInventory = owner->GetTokenInventory();
                size_t tokenParamCount = GetTokenParamCount();
                std::vector<int> availableTokenID;

                // i 는 1부터 시작
                for (size_t i = 1; i <= tokenParamCount; ++i)
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