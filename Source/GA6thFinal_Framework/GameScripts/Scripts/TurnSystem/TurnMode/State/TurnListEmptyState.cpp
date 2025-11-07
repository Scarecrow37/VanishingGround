#include "pchScripts.h"
#include "TurnListEmptyState.h"
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/CharacterBase.h>
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "BattleSystem/Battle.h"
#include "WeaponSystem/WeaponSystem.h"
#include "TutorialSystem/TutorialSystem.h"
#include "QTE/System/QTESystem.h"

REGISTER_CLASS(FSMStateFactory, TurnListEmptyState)

TurnListEmptyState::TurnListEmptyState() {}

TurnListEmptyState::~TurnListEmptyState() {}

void TurnListEmptyState::OnAwake() {}

void TurnListEmptyState::OnStart() 
{
    TurnModeStateBase::OnStart();

}

void TurnListEmptyState::OnEnter() 
{
    auto& currentTurn = _turnMode->GetCurrTurnActor();
    const TurnActor* turnActor = currentTurn.Get();
    if (turnActor && typeid(Player) == typeid(*turnActor))
    {
        // 플레이어면 무기로 적 처치 호출
        UpdateCharacterDead([this](CharacterBase& deadCharacter) 
        { 
            if (typeid(Enemy) == typeid(deadCharacter))
            {
                if (WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance())
                {
                    //마지막 공격 대상
                    std::weak_ptr<Enemy> lastTarget = Battle::GetLastTargetEnemy();

                    //조건 평가를 정확하게 하기 위해 마지막 공격 대상을 죽은 캐릭터로 설정
                    std::weak_ptr<Enemy> deadTarget = deadCharacter.GetWeakPtrAs<Enemy>();
                    Battle::SetLastTargetEnemy(deadTarget);

                    //액션 호출
                    Enemy& enemy = static_cast<Enemy&>(deadCharacter);
                    _turnMode->ApplyActions([&enemy, weaponSystem](TurnAction& action) 
                    {
                        WeaponElement& element = weaponSystem->GetCurrentWeaponElement();
                        action.OnEnemyDeadByWeapon(enemy, element);
                    });

                    //마지막 공격 대상 복원
                    Battle::SetLastTargetEnemy(lastTarget);
                }
            }
        });

        //액션 후 다시 사망 계산
        UpdateCharacterDead();
        if (TutorialSystem* system = SingletonComponent<TutorialSystem>::GetInstance())
        {
            system->Show(805903); // 연격 튜토리얼
            if (QTESystem* qteSystem = SingletonComponent<QTESystem>::GetInstance())
            {
               auto& result = qteSystem->GetQTEOverallResult();
               if (0 < result.MissCount)
               {
                   system->Show(805912); // 미스 튜토리얼
               }
            }
        } 
    }
    else
    {
        UpdateCharacterDead();
    }

    _turnMode->FinishCurrentTurn();
    UmLogger.Message(LogLevel::LEVEL_DEBUG, (const char*)u8"턴 리스트를 확인합니다.");
}

void TurnListEmptyState::OnExit() {}

void TurnListEmptyState::OnUpdate() {}
