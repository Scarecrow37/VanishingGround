#include "pchScripts.h"
#include "GameClearState.h"
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/CharacterBase.h>
#include "ItemDropSystem/ItemDropSystem.h"
#include "RevelationSystem/RevelationSystem.h"
#include "Map/MapManager.h"
#include "SceneTransition/SceneTransitionComponent.h"
#include "Utility/SceneGuid.h"

REGISTER_CLASS(FSMStateFactory, GameClearState)

GameClearState::GameClearState() {}

GameClearState::~GameClearState() {}

void GameClearState::OnAwake() {}

void GameClearState::OnEnter() 
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, u8"게임 클리어!");
    // 엔딩 분기 확인
    if (MapManager* mapManager = SingletonComponent<MapManager>::GetInstance())
    {
        // 스테이지 남은게 있는지 확인
        const bool remaining = mapManager->IsRemainingStage();
        if (remaining)
        {
            if (ItemDropSystem* system = SingletonComponent<ItemDropSystem>::GetInstance())
            {
                system->PlayItemDropUISequence();
            }
            if (_revelationSystem)
            {
                _revelationSystem->RemoveAllExtinctionElements();
            }
        }
        // 남은 스테이지가 없다면 엔딩
        else
        {
            UmTime.Invoke(GetFSM(), 3.0f, [this]() {
                if (auto* sceneTransition = SingletonComponent<SceneTransitionComponent>::GetInstance())
                {
                    auto weakOwner = GetFSM().GetWeakPtr();
                    sceneTransition->SceneTransitionFade("in", "out", [this, weakOwner]() {
                        bool expired = weakOwner.expired();
                        assert(false == expired && "콜백으로 등록한 객체가 댕글링 포인터입니다.");
                        if (false == expired)
                        {
                            UmSceneManager.LoadScene(UmFileSystem.GetPathFromGuid(SceneGuid::ENDING).string());
                        }
                    });
                }
            });
        }
    }
}

void GameClearState::OnExit() {}

void GameClearState::OnUpdate() {}
