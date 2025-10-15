#include "pchScripts.h"
#include "MonsterController.h"
#include "Monster/AI/MonsterAIFactory.h"
#include "Monster/Action/MonsterAction.h"
#include "Monster/Action/MonsterActionFactory.h"

#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

namespace Monster
{
    Controller::Controller() = default;
    Controller::~Controller() = default;

    void Controller::Build(std::weak_ptr<Enemy> weakOwner, const Monster::DataContext* pDataContext,
                           const StageContext* pStageContext)
    {
        _weakOwner = weakOwner;
        if (pDataContext)
        {
            _dataContext  = *pDataContext;
        }
        if (pStageContext)
        {
            _stageContext = *pStageContext;
        }

        BuildAIModel();
        BuildAction();
    }
    void Controller::Clear()
    {
        if (_currAction)
        {
            _currAction->ProcessActionExit();
        }
        _aiModel.Clear();
        _actionTable.clear();
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
        }
        return false;
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

        int randomIndex = Random::Range(0, (int)_dataContext.FsmIDs.size());
        auto func       = AIFactory::GetAIBuildFunc(randomIndex);
        auto owner      = _weakOwner.lock();
        if (func && owner)
        {
            func(owner, _aiModel);
        }
    }
    void Controller::BuildAction() 
    {
        _actionTable.clear();

        for (ActionID id : _dataContext.ActionIDs)
        {
            Action* action = nullptr;
            if (ActionFactory::NewActionFromID(id, &action))
            {
                _actionTable[id] = std::unique_ptr<Action>(action);
            }
        }
    }
    void Controller::SetCurrentAction(ActionID actionID) 
    {
        if (_actionTable.contains(actionID))
        {
            _currAction = _actionTable[actionID].get();
        }
        else
        {
            _currAction = nullptr;
        }
    }
}
