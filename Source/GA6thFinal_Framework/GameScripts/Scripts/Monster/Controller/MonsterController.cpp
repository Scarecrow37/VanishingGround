#include "pchScripts.h"
#include "MonsterController.h"
#include "Monster/AI/MonsterAIFactory.h"
#include "Monster/Action/MonsterActionBase.h"
#include "Monster/Action/MonsterActionFactory.h"

#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

#include "Monster/System/MonsterSystem.h"

namespace Monster
{
    Controller::Controller() = default;
    Controller::~Controller() = default;

    Action::Base* Controller::GetCurrentAction() const
    {
        return _currAction;
    }

    bool Controller::Build(std::weak_ptr<Enemy> weakOwner, const Monster::DataContext* pDataContext,
                           const StatContext* pStatContext)
    {
        assert(weakOwner.expired() == false); // [assert] 컨트롤러의 소유자가 유효해야합니다.
        assert(pDataContext != nullptr);      // [assert] 컨트롤러의 데이터 컨텍스트가 유효해야합니다.
        assert(pStatContext != nullptr);      // [assert] 컨트롤러의 스탯 컨텍스트가 유효해야합니다.
        if (weakOwner.expired() || pDataContext == nullptr || pStatContext == nullptr)
        {
            return false;
        }

        _weakOwner = weakOwner;
        _dataContext = *pDataContext;
        _statContext = *pStatContext;

        BuildAIModel();
        BuildAction();

        int actionID = _aiModel.GetCurrentActionID();
        SetCurrentAction(actionID);

        return true;
    }
    void Controller::Reset()
    {
        _aiModel.Reset();
        for (auto& [_, actionPtr] : _actionIDTable)
        {
            if (actionPtr)
            {
                actionPtr->Reset();
            }
        }
    }
    void Controller::Clear()
    {
        if (_currAction)
        {
            _currAction->ProcessActionExit();
        }
        _aiModel.Clear();
        _actionIDTable.clear();
        _weakOwner.reset();
        _currAction = nullptr;
        _prevAction = nullptr;
    }
    bool Controller::ProcessAction()
    {
        if (_currAction)
        {
            if (_currAction != _prevAction)
            {
                _currAction->ProcessActionEnter();
                _prevAction = _currAction;
            }

            _currAction->ProcessActionUpdate();

            if (_currAction->IsActionEnd())
            {
                _currAction->ProcessActionExit();
                return true;
            }
            return false;
        }
        return true;
    }
    void Controller::ProcessAnimationEvent(const Timeline::EventContext* context)
    {
        if (_currAction)
        {
            _currAction->ProcessAnimationEvent(context);
        }
    }

    void Controller::Transition()
    {
        _aiModel.Transition();
        _aiModel.Refresh();

        int actionID = _aiModel.GetCurrentActionID();
        SetCurrentAction(actionID);
    }

    void Controller::BuildAIModel() 
    {
        _aiModel.Clear();

        size_t index    = Random::Index(_dataContext.FsmIDs.size());
        _fsmID          = _dataContext.FsmIDs[index];
        auto func       = AIFactory::GetAIBuildFunc(_fsmID);
        auto owner      = _weakOwner.lock();
        if (func && owner)
        {
            func(owner, _aiModel);
        }
    }
    void Controller::BuildAction() 
    {
        _actionIDTable.clear();
        for (size_t i = 0; i < _dataContext.ActionIDs.size(); ++i)
        {
            ActionID id = _dataContext.ActionIDs[i];
            if (0 != id)
            {
                Action::Base* action = nullptr;
                if (MonsterSystem* system = SingletonComponent<MonsterSystem>::GetInstance())
                {
                    ActionContext const* pActionContext = system->GetActionContextFromID(id);
                    assert(pActionContext); // [assert] 해당 액션 ID에 대한 액션 컨텍스트가 존재하지 않습니다.
                    if (pActionContext)
                    {
                        if (ActionFactory::NewActionFromID(id, &action))
                        {
                            bool initResult =
                                action->Initialize(_weakOwner, pActionContext, &_statContext.ActionParams[i],
                                                   &_statContext.TokenParams[i]);
                            if (initResult)
                            {
                                action->Reset();
                                // 액션 생성 성공 시 unique_ptr로 관리
                                _actionIDTable[id] = std::unique_ptr<Action::Base>(action);
                            }
                        }
                    }
                }
            }
        }
    }
    void Controller::SetCurrentAction(ActionID actionID) 
    {
        if (_actionIDTable.contains(actionID))
        {
            _currAction = _actionIDTable[actionID].get();
        }
        else
        {
            _currAction = nullptr;
        }
    }
}
