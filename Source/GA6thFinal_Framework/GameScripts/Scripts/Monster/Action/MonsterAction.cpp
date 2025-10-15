#include "pchScripts.h"
#include "MonsterAction.h"
#include "Utility/SingletonHelper.h"
#include "Monster/System/MonsterSystem.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"

namespace Monster
{
    Action::Action(std::weak_ptr<Enemy> owner, const DataContext* pDataContext, const ActionContext* pActionContext,
                   const StageContext* pStageContext)
        : _weakOwner(owner), _dataContext(*pDataContext), _actionContext(*pActionContext), _stageContext(*pStageContext)
    {
    }
    void Action::ProcessActionEnter() 
    {
        _isActionEnd = false;
        Refresh();
        OnActionEnter();
    }
    void Action::ProcessActionUpdate() 
    {
        OnActionUpdate();
    }
    void Action::ProcessActionExit() 
    {
        OnActionExit();
    }

    void Action::Refresh()
    {
        RefreshTarget();
    }
    void Action::RefreshTarget()
    {
        _target = std::weak_ptr<CharacterBase>();
        const std::string& targetStr = _actionContext.Target;

        if (targetStr == "Self")
        {
            _target = std::static_pointer_cast<CharacterBase>(_weakOwner.lock());
        }
        else if (targetStr == "Player")
        {
            if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
            {
                if (Player* player = turnMode->GetPlayer())
                {
                    _target = std::static_pointer_cast<CharacterBase>(player->GetWeakPtr().lock());
                }
            }
        }
        else
        {
            try // ID로 변환 시도
            {
                int targetID = std::stoi(targetStr);
                if (MonsterSystem* system = SingletonComponent<MonsterSystem>::GetInstance())
                {
                    auto weakEnemies = system->GetSpawnedEnemyByID(targetID);
                    for (const auto& weakEnemy : weakEnemies)
                    {
                        if (false == weakEnemy.expired())
                        {
                            auto sharedEnemy = weakEnemy.lock();
                            if (false == sharedEnemy->IsDead())
                            {
                                _target = std::static_pointer_cast<CharacterBase>(sharedEnemy);
                                break; // 첫번째 살아있는 몬스터만 타겟으로 설정
                            }
                        }
                    }
                }
            }
            catch (...)
            {
            }
        }
    }
} // namespace Monster