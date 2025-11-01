#pragma once
#include "../TurnAction/TurnAction.h"
#include "Utility/SingletonHelper.h"
#include "Monster/Common/MonsterCommon.h"

class FiniteStateMachine;
class TurnActor;
class Enemy;
class Player;
class UmCineMotion;

/*
* 턴을 관리하는 컴포넌트입니다.
*/
class TurnMode : public Component
{
    USING_PROPERTY(TurnMode)
public:
    TurnMode();
    virtual ~TurnMode();

public:
    FiniteStateMachine&                 GetFSM() { return *_finiteStateMachine; }
    Player*                             GetPlayer();
    const std::vector<Enemy*>&          GetEnemies();
    const std::vector<CharacterBase*>&  GetCharacters();
    Enemy*                              GetEnemyFromSpawnPoint(Monster::SpawnPoint spawnPoint);

    /// <summary>
    /// 라운드 카운트를 0으로 초기화합니다.
    /// </summary>
    /// <returns></returns>
    void ResetRoundCount() { _roundCount = 0; }

    /// <summary>
    /// 라운드를 증가시킵니다.
    /// </summary>
    /// <returns></returns>
    int AddRoundCount() { return ++_roundCount; }

    /// <summary>
    /// 턴 리스트 생성 및 정렬을 수행합니다./
    /// </summary>
    void MakeTurnList();

    /// <summary>
    /// 가장 우선순위가 높은 TurnActor를 CurrTurnActor 로 설정합니다.
    /// </summary>
    /// <returns>현재 CurrentTurnActor를 반환합니다.</returns>
    void StartFrontTurnActor();


    /// <summary>
    /// TurnList의 첫번째 Actor를 제거하고 현재 턴 Actor를 nullptr로 설정합니다.
    /// </summary>
    void FinishCurrentTurn();

    /// <summary>
    /// 현재 턴 실행중인 Actor를 반환합니다.
    /// </summary>
    const MVVM::Model<TurnActor*>& GetCurrTurnActor() const { return _currTurnActor; }

    /// <summary>
    /// 턴 대기중인 Actor의 개수를 반환합니다.
    /// </summary>
    /// <returns>int 갯수</returns>
    int GetPendingActorCount();

    /*slot 값을 통해 플레이어 엑터인지 확인합니다.*/
    static bool IsPlayerActorSlot(const std::pair<int, TurnActor*>& turnActor)
    {
        auto& [slot, actor] = turnActor;
        return 0 <= slot;
    }

    UmCineMotion* GetIntroCamera() { return _introCamera.lock().get(); }
    UmCineMotion* GetBattleCamera() { return _battleCamera.lock().get(); }

public:
    REFLECT_PROPERTY(
        RoundCount
    )

    GETTER_ONLY(int, RoundCount) { return _roundCount; }
    // 현재 라운드 수
    // type : int
    PROPERTY(RoundCount)

    GETTER(bool, RevelationActiveFlag) { return _revelationActiveFlag;  }
    SETTER(bool, RevelationActiveFlag) 
    { 
       _revelationActiveFlag = value; 
       if (value)
       {
           _currentTurnRevelationActiveFlag = value;
       }
    }
    // 계시 발동 조건 평가용 플래그 변수입니다. QTE 공격이 완전히 종료될때마다 false로 초기화됩니다.
    // type : bool
    PROPERTY(RevelationActiveFlag)

    GETTER_ONLY(bool, IsCurrentTurnActiveRevelation) { return _currentTurnRevelationActiveFlag; }
    // 이번 턴 계시 발동 여부 입니다.
    // type : bool
    PROPERTY(IsCurrentTurnActiveRevelation)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TurnMode)

private:
    void BuildTurnModeFSM();
    void AddRoundOnceActions();

    /*slot 값을 통해 실제 RoundSpeed를 반환합니다.*/
    int GetRealRoundSpeed(const std::pair<int, TurnActor*>& turnActor);

private:
    SingletonComponent<TurnMode> _singletonComponent{this};

    FiniteStateMachine* _finiteStateMachine = nullptr;

    int _roundCount;
    /*플레이어의 무기 slot 번호를 함께 저장합니다. int 값이 -1이면 Enemy, 0 이상이면 Player 입니다.*/
    MVVM::Model<std::deque<std::pair<int, TurnActor*>>> _turnList;
    MVVM::Model<TurnActor*>                             _currTurnActor;

    /*이번 공격에 대한 계시 발동 여부를 관리하는 플래그입니다.*/
    bool _revelationActiveFlag = false;
    /*이번 턴에 대한 계시 발동 여부를 관리하는 플래그입니다.*/
    bool _currentTurnRevelationActiveFlag = false;

    // 카메라 연출용
    std::weak_ptr<UmCineMotion> _introCamera;
    std::weak_ptr<UmCineMotion> _battleCamera;

private:
    struct SystemStates
    {
        class CombatStartPhase*   CombatStartPhase   = nullptr;
        class RoundStartPhase*    RoundStartPhase    = nullptr;
        class RoundEndPhase*      RoundEndPhase      = nullptr;
        class PlayerActionPhase*  PlayerActionPhase  = nullptr;
        class EnemyActionPhase*   EnemyActionPhase   = nullptr;
        class CheckPlayerState*   CheckPlayerState   = nullptr;
        class TurnListEmptyState* TurnListEmptyState = nullptr;
        class GameOverState*      GameOverState      = nullptr;
        class GameClearState*     GameClearState     = nullptr;
    } _systemStates;

    struct SystemCondition
    {
        class CombatStartCodition*   CombatStartCodition   = nullptr;
        class RoundStartCondition*   RoundStartCondition   = nullptr;
        class PlayerActionCondition* PlayerActionCondition = nullptr;
        class EnemyActionCondition*  EnemyActionCondition  = nullptr;
        class CheckRoundStartExit*   CheckRoundStartExit   = nullptr;
        class CheckRoundEndExit*     CheckRoundEndExit     = nullptr;
        class CheckTurnEndCondition* CheckTurnEndCondition = nullptr;
        class CheckTurnEmpty*        CheckTurnEmpty        = nullptr;
        class CheckTurnNotEmpty*     CheckTurnNotEmpty     = nullptr;
        class GameOverCondition*     GameOverCondition     = nullptr;
        class GameClearCondition*    GameClearCondition    = nullptr;
    } _systemConditions;

public:
    /// <summary>
    /// 실제 활성화된 Action들 순회하면서 함수를 실행시킵니다.
    /// </summary>
    /// <returns></returns>
    void ApplyActions(const std::function<void(TurnAction& action)>& func) 
    {
        std::erase_if(_turnActions, [&func](const auto& pair) 
        {
            bool result = true;
            auto& [isDestroy, action] = pair;
            if (isDestroy)
            {
                result = *isDestroy;
                if (false == result)
                {
                    func(*action);
                }
            }
            return result;
        });
    };

    /// <summary>
    /// 턴 라이프 사이클에 액션을 추가합니다.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="action :">해당 객체 포인터</param>
    /// <returns></returns>
    template <typename T>
    bool AddTurnAction(T* action)
    {
        static_assert(std::is_base_of_v<TurnAction, std::remove_cvref_t<T>>, "T is not derived from TurnAction.");
        bool result = false;
        if (nullptr != action)
        {
            if (false == action->IsValidAction())
            {
                auto& [isDestroy, newAction] = _turnActions.emplace_back();
                isDestroy.reset(new bool{false});
                TurnAction* baseAction = static_cast<TurnAction*>(action);
                baseAction->_isDestroy = isDestroy.get();
                newAction              = baseAction;
                result                 = true;
                CallAddedAction(action);
            }
        }
        return result;
    }
private:
    void CallAddedAction(TurnAction* action);

private:
    std::vector<std::pair<std::unique_ptr<bool>, TurnAction*>> _turnActions;

public:
    GETTER_ONLY(const SystemStates&, States) { return _systemStates; }
    /// <summary>
    /// TurnMode FSM의 State 객체들 입니다.
    /// </summary>
    PROPERTY(States)

    GETTER_ONLY(const SystemCondition&, Conditions) { return _systemConditions; }
    /// <summary>
    /// TurnMode용 FSM의 Condition 객체들 입니다.
    /// </summary>
    PROPERTY(Conditions)

protected:
    void Reset() override;


    /// <summary>
    /// <para> 이 함수는 항상 Start 함수 전에 호출되며 프리팹이 인스턴스화 된 직후에 호출됩니다.                </para>
    /// <para> 게임 오브젝트의 Active가 false 상태인 경우 Awake 함수는 true가 될때까지 호출되지 않습니다.      </para>
    /// </summary>
    virtual void Awake() override;

    virtual void ImGuiDrawPropertysEvent() override;

    void FindCameras();

};
