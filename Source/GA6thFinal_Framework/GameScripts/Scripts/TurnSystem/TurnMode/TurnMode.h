#pragma once
#include "UmFramework.h"
#include "../TurnAction/TurnAction.h"

class FiniteStateMachine;
class TurnActor;
class Enemy;
class Player;

/*
* 턴을 관리하는 컴포넌트입니다.
*/
class TurnMode : public Component
{
    USING_PROPERTY(TurnMode)
    inline static TurnMode* static_instance = nullptr;
public:
    static TurnMode* GetInstance() 
    { 
        if (nullptr == static_instance)
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Turn Mode가 존재하지 않습니다.");
        }
        return static_instance;
    }

public:
    TurnMode();
    virtual ~TurnMode();

public:
    FiniteStateMachine& GetFSM() { return *_finiteStateMachine; }

public:
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
    /// 현재 Scene에 존재하는 모든 TurnActor를 TurnList에 담습니다.
    /// </summary>
    void MakeTurnList();

    /// <summary>
    /// Random Speed를 뽑고 TurnList를 정렬합니다.
    /// </summary>
    void SortTurnList();

    /// <summary>
    /// 가장 우선순위가 높은 TurnActor를 List에서 지우고 CurrTurnActor 로 설정합니다.
    /// </summary>
    /// <returns></returns>
    TurnActor* PopTurnList();

    /// <summary>
    /// 현재 턴 실행중인 Actor를 반환합니다.
    /// </summary>
    TurnActor* GetCurrTurnActor() const { return _currTurnActor; }

    /// <summary>
    /// 턴 대기중인 Actor의 개수를 반환합니다.
    /// </summary>
    /// <returns></returns>
    int GetPendingActorCount();

public:
    struct Battle
    {
        /// <summary>
        /// 플레이어로 공격을 수행합니다.
        /// </summary>
        /// <param name="attacker :">공격자</param>
        /// <param name="target :">대상</param>
        void operator()(Player& attacker, Enemy& target);

        /// <summary>
        /// 적으로 공격을 수행합니다.
        /// </summary>
        /// <param name="attacker :">공격자</param>
        /// <param name="target :">대상</param>
        void operator()(Enemy& attacker, Player& target);

        /// <summary>
        /// 마지막으로 공격한 CharacterBase를 반환합니다.
        /// </summary>
        /// <returns></returns>
        static const std::weak_ptr<CharacterBase>& GetLastAttacker() { return lastAttacker; }

        /// <summary>
        /// 마지막으로 공격당한 CharacterBase를 반환합니다
        /// </summary>
        /// <returns></returns>
        static const std::weak_ptr<CharacterBase>& GetLastTarget() { return lastTarget; }

        /// <summary>
        /// 마지막으로 공격당한 적을 반환합니다
        /// </summary>
        /// <returns></returns>
        static const std::weak_ptr<Enemy>& GetLastTargetEnemy() { return lastTargetEnemy; }

        /// <summary>
        /// 마지막으로 공격당한 적을 기록하는 변수를 초기화합니다. 전투 시작시 초기화됩니다.
        /// </summary>
        inline static void ResetLastCharacter()
        {
            lastAttacker    = std::weak_ptr<CharacterBase>();
            lastTarget      = std::weak_ptr<CharacterBase>();
            lastTargetEnemy = std::weak_ptr<Enemy>();
        }

    private:
        inline static std::weak_ptr<CharacterBase> lastAttacker;
        inline static std::weak_ptr<CharacterBase> lastTarget;
        inline static std::weak_ptr<Enemy>         lastTargetEnemy;
    };

public:
    REFLECT_PROPERTY(
        RoundCount
    )

    GETTER_ONLY(int, RoundCount) { return _roundCount; }
    PROPERTY(RoundCount)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TurnMode)

private:
    void BuildTurnModeFSM();

    /*slot 값을 통해 플레이어 엑터인지 확인합니다.*/
    bool IsPlayerActorSlot(const std::pair<int, TurnActor*>& turnActor)
    {
        auto& [slot, actor] = turnActor;
        return 0 <= slot;
    }

    /*slot 값을 통해 실제 RoundSpeed를 반환합니다.*/
    int GetRealRoundSpeed(const std::pair<int, TurnActor*>& turnActor);

private:
    FiniteStateMachine* _finiteStateMachine = nullptr;

    int _roundCount;
    /*플레이어의 무기 slot 번호를 함께 저장합니다. int 값이 -1이면 Enemy, 0 이상이면 Player 입니다.*/
    std::deque<std::pair<int, TurnActor*>> _turnList;
    TurnActor* _currTurnActor;

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
        static_assert(std::is_base_of_v<TurnAction, T>, "T is not derived from TurnAction.");
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
            }
        }
        return result;
    }

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

};
