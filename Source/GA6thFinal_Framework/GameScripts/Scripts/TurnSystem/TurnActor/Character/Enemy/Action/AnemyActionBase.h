#pragma once
#include "Animation/Structs/AnimationData.h"

class Enemy;
class SkeletalMeshRenderer;
class AnimationComponent;
class CharacterBase;

namespace EnemyAction
{
    /// <summary>
    /// 찢어 발기기
    /// </summary>
    class ActionBase
    {
    public:
        ActionBase(Enemy* owner);
        virtual ~ActionBase() = default;

    public:
        virtual void OnActionEnter()    = 0;
        virtual bool OnActionUpdate()   = 0;
        virtual void OnActionExit()     = 0;

    protected:
        void ProcessBattle();

    protected:
        Enemy* _owner = nullptr;
        AnimationComponent* _animator = nullptr;

    };
} // namespace Enemy