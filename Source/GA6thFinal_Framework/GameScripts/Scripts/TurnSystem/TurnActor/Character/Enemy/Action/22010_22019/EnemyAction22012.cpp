#include "pchScripts.h"
#include "EnemyAction22012.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Mesh/SkeletalMeshRenderer.h>

namespace EnemyAction
{
    void Action22012::OnActionEnter() 
    {
        SetAnimation(CharacterBase::ATTACK_1, false);
    }
    bool Action22012::OnActionUpdate() 
    {
        return isAnimationEnd();
    }
    void Action22012::OnActionExit() 
    {
    }
} // namespace EnemyAction