#include "pchScripts.h"
#include "MonsterActionTearApart.h"
#include <Animation/AnimationComponent.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>

REGISTER_MONSTER_ACTION(Monster::Action::TearApart)
namespace Monster
{
    namespace Action
    {
        void TearApart::OnActionEnter() {}
        void TearApart::OnActionUpdate() {}
        void TearApart::OnActionExit() {}
        void TearApart::OnActionReset() {}
    } // namespace Action
} // namespace Monster