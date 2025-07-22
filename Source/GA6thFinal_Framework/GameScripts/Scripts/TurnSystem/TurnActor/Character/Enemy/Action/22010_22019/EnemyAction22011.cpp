#include "pchScripts.h"
#include "EnemyAction22011.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Mesh/SkeletalMeshRenderer.h>

namespace EnemyAction
{
    void Action22011::OnActionEnter() 
    {
        SetMainAnimation(CharacterBase::ATTACK_1, false);
    }
    bool Action22011::OnActionUpdate() 
    {
        return IsAnimationEnd();
    }
    void Action22011::OnActionExit() 
    {
    }
} // namespace EnemyAction