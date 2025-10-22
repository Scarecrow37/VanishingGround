#include "pchScripts.h"
#include "MonsterActionCourtesyOfTheKing.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

REGISTER_MONSTER_ACTION(Monster::Action::CourtesyOfTheKing)
namespace Monster
{
    namespace Action
    {
        CourtesyOfTheKing::CourtesyOfTheKing() : Base("Attack1") {}

        CourtesyOfTheKing::~CourtesyOfTheKing() = default;

        void CourtesyOfTheKing::OnActionEnter() {}

        void CourtesyOfTheKing::OnActionUpdate() {}

        void CourtesyOfTheKing::OnActionExit() {}

        void CourtesyOfTheKing::OnActionReset() {}

        void CourtesyOfTheKing::OnNotifiedAnimationEvent(const Timeline::EventContext* context)
        {
            const std::string& label = context->GetLabel();
            if ("Behavior" == label)
            {
                Behavior();
            }
        }

        /*
        바른에게 T_Param(1)을 부여한다.
        */
        void CourtesyOfTheKing::Behavior()
        {
            TokenParam tokenParam = GetTokenParam(1);
            if (CharacterBase* target = GetTarget())
            {
                TokenInventory& tokenInventory = target->GetTokenInventory();
                tokenInventory.AddTokenStackFromID(tokenParam.TokenID, tokenParam.Count);
            }
        }
    } // namespace Action
} // namespace Monster