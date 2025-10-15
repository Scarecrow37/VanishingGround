#pragma once
#include "Monster/AI/MonsterAIModel.h"

class Enemy;

namespace Monster
{
    class Action;

    class Controller final
    {
        using ActionTable = std::unordered_map<ActionID, std::unique_ptr<Action>>;

    public:
        Controller() = default;
        ~Controller() = default;

    public:
        inline bool Invalid() { return _weakOwner.expired(); }
        inline Enemy* GetOwner() { return _weakOwner.lock().get(); }

        void Build(std::weak_ptr<Enemy> weakOwner, const Monster::DataContext* pDataContext, const StageContext* pStageContext);

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
        Monster::StageContext _stageContext;

        AIModel     _aiModel;
        Action*     _currAction = nullptr;
        Action*     _prevAction = nullptr;
        ActionTable _actionTable;

    };
}
