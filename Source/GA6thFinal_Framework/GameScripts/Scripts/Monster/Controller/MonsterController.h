#pragma once
#include "Monster/Context/MonsterDataContext.h"
#include "Monster/Context/MonsterStatContext.h"
#include "Monster/Context/MonsterSpawnContext.h"
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
        inline const Monster::DataContext&      GetDataContext() const { return _dataContext; }
        inline const Monster::StatContext&      GetStateContext() const { return _statContext; }
        inline const AIModel&   GetAIModel() const { return _aiModel; }
        inline bool             Invalid() const { return _weakOwner.expired(); }
        inline Enemy*           GetOwner() const { return _weakOwner.lock().get(); }
        inline FSMID            GetFSMID() const { return _fsmID; }
        Action::Base*           GetCurrentAction() const;


        bool Build(std::weak_ptr<Enemy> weakOwner, const Monster::DataContext* pDataContext,
                   const StatContext* pStatContext, const SpawnContext* pSpawnContext);

        void Reset();

        void Clear();

        /// <summary></summary>
        bool ProcessAction();
        void ProcessAnimationEvent(const Timeline::EventContext* context);

        /// <summary>AI FSM의 상태를 전이합니다.</summary>
        void Transition();

        /// <summary>초기 토큰을 설정합니다.</summary>
        void SetInitialToken();

    private:
        void BuildAIModel();
        void BuildAction();

        void SetCurrentAction(ActionID actionID);

    private:
        std::weak_ptr<Enemy>    _weakOwner;
        Monster::DataContext    _dataContext;
        Monster::StatContext    _statContext;
        Monster::SpawnContext   _spawnContext;

        FSMID                   _fsmID = 0;
        AIModel                 _aiModel;

        Action::Base*           _currAction = nullptr;
        Action::Base*           _prevAction = nullptr;
        std::unordered_map<ActionID, std::unique_ptr<Action::Base>> _actionIDTable;

    };
}
