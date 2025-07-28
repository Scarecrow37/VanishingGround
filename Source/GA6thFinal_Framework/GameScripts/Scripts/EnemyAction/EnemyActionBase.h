#pragma once

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
        void RequireActionEnter();
        void RequireActionUpdate();
        void RequireActionExit();

        inline void SetActionEnd() { _isActionEnd = true; }
        inline bool IsActionEnd() const { return _isActionEnd; }

    private:
        virtual void OnActionEnter()    = 0;
        virtual void OnActionUpdate() = 0;
        virtual void OnActionExit()     = 0;

    protected:
        void ProcessBattle();

    protected:
        Enemy*              _owner = nullptr;
        AnimationComponent* _animator = nullptr;
        bool                _isActionEnd = false;

    };
} // namespace Enemy