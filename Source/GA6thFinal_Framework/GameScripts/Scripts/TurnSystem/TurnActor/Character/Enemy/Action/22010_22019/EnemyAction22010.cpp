#include "pchScripts.h"
#include "EnemyAction22010.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Mesh/SkeletalMeshRenderer.h>

namespace EnemyAction
{
    void Action22010::OnActionEnter() 
    {
        SetAnimation(CharacterBase::ATTACK_1, false);
    }

    bool Action22010::OnActionUpdate()
    {
        return isAnimationEnd();
    }

    void Action22010::OnActionExit() 
    {
    }
} // namespace EnemyAction