#include "pchScripts.h"
#include "EnemyPlayTurnState.h"

#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include "TurnSystem/TurnMode/TurnMode.h"
#include "RoundInfoUI/RoundInfoUIManager.h"
#include "Monster/Action/MonsterActionBase.h"
#include "TurnSystem/TurnMode/State/EnemyActionPhase.h"

REGISTER_CLASS(FSMStateFactory, EnemyPlayTurnState)

REFLECT_FUNCTION(EnemyPlayTurnState)

// switch문 안에 있어야함
// AI 모델 ID값에 해당하는 AI 모델을 셋업
#define BUILD_AI(index)             \
case index:                         \
BuildAIModel##index();              \
break;

void EnemyPlayTurnState::OnAwake() 
{
}

void EnemyPlayTurnState::OnStart() 
{
    _roundInfoUIManager = GameObject::FindComponentWithTag<RoundInfoUIManager>("Round Info Panel");
}

void EnemyPlayTurnState::OnEnter() 
{
    _isOnce = false;

    Enemy& enemy = GetEnemy();
    Monster::Controller& controller = enemy.GetController();

    std::string spawnPoint = Monster::SpawnPointToString(enemy.SpawnPoint);
    std::string actionName = STR_NULL;

    const std::string message = std::format("{}{}{}{}", spawnPoint, (const char*)u8" Enemy 턴 시작. ",
                                            (const char*)u8"Action : ", actionName);
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);
}

void EnemyPlayTurnState::OnExit() 
{
    Enemy& enemy = GetEnemy();
    std::string spawnPoint = Monster::SpawnPointToString(enemy.SpawnPoint);
    std::string message = std::format("{} {}{}", spawnPoint, enemy.gameObject->ToString(), (const char*)u8" 턴 종료.");
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);

    if (TurnMode* mode = SingletonComponent<TurnMode>::GetInstance())
    {
        mode->ApplyActions([this](TurnAction& action) { action.OnTurnEnd(GetEnemy()); });
    }
}
    
void EnemyPlayTurnState::OnUpdate()
{
    if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
    {
        if (EnemyActionPhase* waitPhase = turnMode->States->EnemyActionPhase)
        {
            if (false == waitPhase->WaitPhase)
            {
                Enemy&               enemy      = GetEnemy();
                Monster::Controller& controller = enemy.GetController();
                bool                 succeed    = controller.ProcessAction(); // 컨트롤러가 액션을 마치면 true를 반환함.
                if (succeed)
                {
                    enemy.EndTurn();
                }

                if (false == _isOnce)
                {
                    if (Monster::Action::Base* action = controller.GetCurrentAction())
                    {
                        const std::string& actionName = action->GetActionContext().Name;
                        if (auto roundInfo = _roundInfoUIManager.lock())
                        {
                            roundInfo->FadeInfoUI(actionName);
                        }
                        _isOnce = true;
                    }
                }  
            }     
        }
    }
}

void EnemyPlayTurnState::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
{
    Enemy& enemy = GetEnemy();
    Monster::Controller& controller = enemy.GetController();
    controller.ProcessAnimationEvent(context);
}