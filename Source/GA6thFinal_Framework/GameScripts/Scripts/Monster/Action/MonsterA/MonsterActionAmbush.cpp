#include "pchScripts.h"
#include "MonsterActionAmbush.h"

REGISTER_MONSTER_ACTION(Monster::Action::Ambush)
namespace Monster
{
    namespace Action
    {
        void Ambush::OnActionEnter()
        {
            if (ProcessAnimation("Attack2"))
            {
                SetActionEnd();
            }
        }
        void Ambush::OnActionUpdate() {}
        void Ambush::OnActionExit() {}
        void Ambush::OnActionReset() {}
    }
} // namespace Monster