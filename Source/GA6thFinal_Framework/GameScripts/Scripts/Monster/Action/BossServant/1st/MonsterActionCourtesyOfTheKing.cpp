#include "pchScripts.h"
#include "MonsterActionCourtesyOfTheKing.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

REGISTER_MONSTER_ACTION(Monster::Action::CourtesyOfTheKing)
namespace Monster
{
    namespace Action
    {
        CourtesyOfTheKing::CourtesyOfTheKing() : BossServantAction("Attack1") {}
        CourtesyOfTheKing::~CourtesyOfTheKing() = default;
        void CourtesyOfTheKing::OnNotifiedAnimationEvent(const Timeline::EventContext* context)
        {
            const std::string& label = context->GetLabel();
            if ("Behavior" == label)
            {
                Behavior();
            }
        }

        /*
        바른에게 T_Param(1)을 부여한다
        자신에게 T_Param(2)를 S_Param(1)~S_Param(2) 만큼 부여한다.
        */
        void CourtesyOfTheKing::Behavior()
        {
            {   // 바른에게 T_Param(1)을 부여한다.
                TokenParam tokenParam = GetTokenParam(1);
                if (CharacterBase* target = GetTarget())
                {
                    TokenInventory& tokenInventory = target->GetTokenInventory();
                    tokenInventory.AddTokenStackFromID(tokenParam.TokenID, tokenParam.Count);
                }
            }
        }
    } // namespace Action
} // namespace Monster