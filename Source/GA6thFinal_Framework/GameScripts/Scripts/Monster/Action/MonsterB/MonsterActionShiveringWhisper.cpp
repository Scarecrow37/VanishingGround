#include "pchScripts.h"
#include "MonsterActionShiveringWhisper.h"

REGISTER_MONSTER_ACTION(Monster::Action::ShiveringWhisper)
namespace Monster
{
    namespace Action
    {
        void ShiveringWhisper::OnActionEnter()
        {
            if (ProcessAnimation("Attack0"))
            {
                SetActionEnd();
            }
        }
        void ShiveringWhisper::OnActionUpdate() {}
        void ShiveringWhisper::OnActionExit() {}
        void ShiveringWhisper::OnActionReset() {}
        void ShiveringWhisper::OnNotifiedAnimationEvent(const Timeline::EventContext* context)
        {
            const std::string& label = context->GetLabel();
            if ("Attack_1" == label)
            {
                Attack();
            }
        }
        void ShiveringWhisper::Attack() {}
    }
} // namespace Monster