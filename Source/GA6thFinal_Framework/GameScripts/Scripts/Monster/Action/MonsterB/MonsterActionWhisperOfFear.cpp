#include "pchScripts.h"
#include "MonsterActionWhisperOfFear.h"
#include <Animation/AnimationComponent.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>

REGISTER_MONSTER_ACTION(Monster::Action::WhisperOfFear)
namespace Monster
{
    namespace Action
    {
        void WhisperOfFear::OnActionEnter() 
        {
            if (ProcessAnimation("Attack0"))
            {
                SetActionEnd();
            }
            if (TokenApplyAction* token = GetTokenAction(1))
            {
                // TODO: 토큰 대상 처리
                // 대상: Player
            }
            if (TokenApplyAction* token = GetTokenAction(2))
            {
                // TODO: 토큰 대상 처리
                // 대상: Self
            }
        }
        void WhisperOfFear::OnActionUpdate() {}
        void WhisperOfFear::OnActionExit() {}
        void WhisperOfFear::OnActionReset() {}
        void WhisperOfFear::OnNotifiedAnimationEvent(const Timeline::EventContext* context)
        {
            const std::string& label = context->GetLabel();
            if ("Attack" == label)
            {
                Attack();
            }
        }
        void WhisperOfFear::Attack() 
        {
            ActionParam damage = GetActionParam(1);
            if (BeginTokenActions())
            {
                ProcessBattle(damage.Param);
                EndTokenActions();
            }
        }
    } // namespace Action
} // namespace Monster