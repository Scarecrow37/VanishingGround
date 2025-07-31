#include "pchScripts.h"
#include "EnemyAction22010.h"
#include <Animation/AnimationComponent.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnAction/Token/TokenApplyAction.h>
#include <Token/Object/Bleed/BleedToken.h>
namespace EnemyAction
{
    Action22010::Action22010(Enemy* owner) 
        : ActionBase(owner)
    {
        tokenAction = new TokenApplyAction();
        tokenAction->TokenID = TokenObject::Bleed::ID;
        tokenAction->TokenCount = 1;
    }
    Action22010::~Action22010() 
    {
        if (tokenAction)
        {
            delete tokenAction;
            tokenAction = nullptr;
        }
    }

    void Action22010::OnActionEnter() 
    {
        if (_animator)
        {
            _animator->BeginBuildOverrideAnimation();
            {
                const char* animKey = _owner->GetAnimationName(CharacterBase::ATTACK_1);
                _animator->ClearOverrideAnimations();
                _animator->PushOverrideAnimation(animKey, true, [](const AnimationData& data) { return data.IsEnd(); });
                _animator->SetCurrentAnimationPopCallback([this]() { SetActionEnd(); });
                _animator->ChangeCurrentAnimationFlags(ANIMATION_FLAG_ALWAYS_UPDATE);
            }
            _animator->EndBuildOverrideAnimation();
        }
    }

    void EnemyAction::Action22010::OnActionUpdate()
    {
    }

    void Action22010::OnActionExit() 
    {
    }

    void Action22010::OnAnimationEvent(const Timeline::EventContext* context) 
    {
        const std::string& label = context->GetLabel();
        if ("Attack" == label)
        {
            TurnMode* turnMode = TurnMode::GetInstance();
            if (turnMode)
            {
                turnMode->AddTurnAction(tokenAction);
                ProcessBattle(5);
                tokenAction->SetDestroy();
            }
        }
    }
} // namespace EnemyAction