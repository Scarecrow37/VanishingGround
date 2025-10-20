#pragma once
#include "Monster/Context/MonsterDataContext.h"
#include "Monster/Context/MonsterStatContext.h"
#include "Monster/AI/MonsterAIModel.h"

class Enemy;

namespace Monster
{
    namespace Action
    {
        class Base;
    }

    class Controller final
    {
    public:
        Controller();
        ~Controller();

    public:
        inline bool Invalid() { return _weakOwner.expired(); }
        inline Enemy* GetOwner() { return _weakOwner.lock().get(); }

        bool Build(std::weak_ptr<Enemy> weakOwner, const Monster::DataContext* pDataContext, const StatContext* pStatContext);

        void Reset();

        void Clear();

        /// <summary></summary>
        bool ProcessAction();

        /// <summary>AI FSM의 상태를 전이합니다.</summary>
        void Transition();

    private:
        void BuildAIModel();
        void BuildAction();

        void SetCurrentAction(ActionID actionID);

    private:
        std::weak_ptr<Enemy>  _weakOwner;
        Monster::DataContext  _dataContext;
        Monster::StatContext  _statContext;

        AIModel         _aiModel;
        Action::Base*   _currAction = nullptr;
        Action::Base*   _prevAction = nullptr;
        std::unordered_map<ActionID, std::unique_ptr<Action::Base>> _actionIDTable;
        std::unordered_map<size_t, Action::Base> _actionIndexTable;

    };
}
