#pragma once
#include "Monster/Context/MonsterDataContext.h"
#include "Monster/Context/MonsterActionContext.h"
#include "Monster/Context/MonsterStatContext.h"

class CharacterBase;
class Enemy;

// @brief 몬스터 액션의 기본 멤버입니다. 이걸 추가하지 않으면 Factory에 추가가 불가능합니다
#define MONSTER_ACTION_DATA(id, key)                                                                                            \
public:                                                                                                                 \
    static constexpr int            ID   = id;                                                                          \
    static constexpr const char8_t* NAME = u8##key;                                                                     \
    inline static int               GetTokenID() const override                                                         \
    {                                                                                                                   \
        return ID;                                                                                                      \
    }                                                                                                                   \
    inline static const char*       GetActionName() const override                                                      \
    {                                                                                                                   \
        return (const char*)NAME;                                                                                       \
    }                                                                                                                   \
    
namespace Monster
{
    class Action
    {
    public:
        Action(std::weak_ptr<Enemy> owner,
               const DataContext* pDataContext, 
               const ActionContext* pActionContext, 
               const StatContext* pStatContext);
        virtual ~Action();

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
        const StatContext&   _statContext;

        int                             _actionID = 0;
        std::weak_ptr<CharacterBase>    _target;
        std::weak_ptr<Enemy>            _weakOwner;

        bool _isActionEnd = false;
    };
}