#pragma once

class Enemy;

namespace EnemyAction
{
    /// <summary>
    /// 찢어 발기기
    /// </summary>
    class ActionBase
    {
    public:
        ActionBase(Enemy* _owner) : _owner(_owner) {}
        virtual ~ActionBase() = default;

    public:
        virtual void OnActionEnter()    = 0;
        virtual bool OnActionUpdate()   = 0;
        virtual void OnActionExit()     = 0;

    protected:
        Enemy* _owner = nullptr;
    };
} // namespace Enemy