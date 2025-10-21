#include "pchScripts.h"
#include "MonsterActionRendApart.h"

REGISTER_MONSTER_ACTION(Monster::Action::RendApart)
namespace Monster
{
    namespace Action
    {
        RendApart::RendApart() : Base("Attack2") {}
        RendApart::~RendApart() = default;

        void RendApart::OnActionEnter() {}

        void RendApart::OnActionUpdate() {}

        void RendApart::OnActionExit() {}

        void RendApart::OnActionReset() {}

        void RendApart::OnNotifiedAnimationEvent(const Timeline::EventContext* context)
        {
            const std::string& label = context->GetLabel();
            if ("Attack" == label)
            {
                Attack();
            }
        }

        /*
        플레이어게 S_Param(1)의 데미지로 피해를 준다.
        */
        void RendApart::Attack()
        {
            ActionParam damage = GetActionParam(1);
            ProcessBattle(damage.Param);
        }
    } // namespace Action
} // namespace Monster