#pragma once
#include <EnemyAction/ActionData/EnemyActionData.h>

class Enemy;
class SkeletalMeshRenderer;
class AnimationComponent;
class CharacterBase;

namespace EnemyAction
{
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
        virtual void OnActionUpdate()   = 0;
        virtual void OnActionExit()     = 0;

    protected:
        void ProcessBattle(int damage, float damageScale = 1.0f);

    protected:
        Enemy*              _owner = nullptr;
        AnimationComponent* _animator = nullptr;
        bool                _isActionEnd = false;

        EnemyAction::ActionData _actionData;
    };
} // namespace Enemy