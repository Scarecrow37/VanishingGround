#pragma once

class Enemy;
class SkeletalMeshRenderer;
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
        void SetAnimation(int animType, bool loop = true, bool blend = true);
        bool isAnimationEnd();

    protected:
        Enemy* _owner = nullptr;
        SkeletalMeshRenderer* _renderer = nullptr;
    };
} // namespace Enemy