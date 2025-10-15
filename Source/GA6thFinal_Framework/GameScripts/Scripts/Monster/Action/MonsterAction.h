#pragma once
#include "Monster/Context/MonsterDataContext.h"
#include "Monster/Context/MonsterActionContext.h"
#include "Monster/Context/MonsterStageContext.h"

class CharacterBase;
class Enemy;

namespace Monster
{
    class Action
    {
    public:
        Action(std::weak_ptr<Enemy> owner,
               const DataContext* pDataContext, 
               const ActionContext* pActionContext, 
               const StageContext* pStageContext);
        virtual ~Action() = default;

    public:
        inline int                          GetActionID()   const { return _actionID; }
        inline std::weak_ptr<Enemy>         GetOwner()      const { return _weakOwner; }
        inline std::weak_ptr<CharacterBase> GetTarget()     const { return _target; }

        void ProcessActionEnter();
        void ProcessActionUpdate();
        void ProcessActionExit();

        inline void SetActionEnd() { _isActionEnd = true; }
        inline bool IsActionEnd() const { return _isActionEnd; }

    private:
        virtual void OnActionEnter()    = 0;
        virtual void OnActionUpdate()   = 0;
        virtual void OnActionExit()     = 0;

        void Refresh();
        void RefreshTarget();

    private:
        const DataContext&   _dataContext;
        const ActionContext& _actionContext;
        const StageContext&  _stageContext;

        int                             _actionID = 0;
        std::weak_ptr<CharacterBase>    _target;
        std::weak_ptr<Enemy>            _weakOwner;

        bool _isActionEnd = false;
    };
}