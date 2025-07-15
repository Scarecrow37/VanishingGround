#pragma once
#include "UmFramework.h"

class CharacterBase;

class TurnActor : public Component
{
    USING_PROPERTY(TurnActor)
public:
    inline static constexpr const char* TAG = "TurnActor";
    struct DEFINE
    {
        inline static constexpr int RANDOMSPEED_MAX = 6;
        inline static constexpr int RANDOMSPEED_MIN = 0;
        inline static constexpr int ROUNDSPEED_MAX = 99;
        inline static constexpr int ROUNDSPEED_MIN = -99;
    };
    enum class STATE
    {
        // 액터가 사망한 상태입니다.
        Dead,
        // 액터가 턴을 기다리는 상태입니다.
        Wait,
        // 액터가 턴을 진행중인 상태입니다.
        Play,
    };

public:
    REFLECT_PROPERTY(
        State,
        RandomSpeed, 
        RoundSpeed,
        IsMyTurn
        )

public:
    TurnActor();
    virtual ~TurnActor();

public:
    /// <summary>
    /// 현재 엑터의 상태를 반환합니다.
    /// </summary>
    /// <returns></returns>
    TurnActor::STATE GetActorState() const { return _currState; }

    /// <summary>
    /// TurnActor의 턴을 시작합니다. OnTurnStart를 호출합니다.
    /// </summary>
    virtual void PlayTurn();

    /// <summary>
    /// Actor의 상태를 초기화합니다. (부활)
    /// OnRevive를 호출합니다.
    /// </summary>
    virtual void Revive();

    /// <summary>
    /// Actor를 사망 상태로 합니다.
    /// OnDead를 호출합니다.
    /// </summary>
    virtual void Dead();

public:
    virtual int GetSpeed() = 0;
    virtual int GetRandomSpeed() { return _randomSpeed; }

protected:
    /// <summary>
    /// 턴 종료를 요청합니다. OnTurnEnd를 호출합니다.
    /// </summary>
    virtual void EndTurn();

public:
    GETTER_ONLY(int, RandomSpeed) { return _randomSpeed; }
    //TurnActor에서 OnRoundStart 진입시 자동으로 랜덤한 값이 부여됩니다.
    PROPERTY(RandomSpeed)
    //void SetRandomSpeed(int randomSpeed) 
    //{ 
    //    _randomSpeed = randomSpeed; 
    //    _randomSpeed = std::clamp(_randomSpeed, DEFINE::RANDOMSPEED_MIN, DEFINE::RANDOMSPEED_MAX);
    //}

    GETTER_ONLY(int, RoundSpeed) 
    { 
        int roundSpeed = GetSpeed() + GetRandomSpeed();
        return std::clamp(roundSpeed, DEFINE::ROUNDSPEED_MIN, DEFINE::ROUNDSPEED_MAX);
    }
    PROPERTY(RoundSpeed)

    //SETTER(STATE, State) { _currState = value; }
    GETTER_ONLY(STATE, State) { return _currState; }
    /*
    * 현재 상태입니다.
    */
    PROPERTY(State)

    /*
    * 현재 턴 여부를 반환합니다.
    */
    GETTER_ONLY(bool, IsMyTurn) { return _currState == STATE::Play; }
    PROPERTY(IsMyTurn)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TurnActor)

private:
    int _randomSpeed = 0;
    STATE _currState;

protected:
    /// <summary>
    /// <para> 이 함수는 항상 Start 함수 전에 호출되며 프리팹이 인스턴스화 된 직후에 호출됩니다.                </para>
    /// <para> 게임 오브젝트의 Active가 false 상태인 경우 Awake 함수는 true가 될때까지 호출되지 않습니다.      </para>
    /// </summary>
    virtual void Awake();

public:
    virtual void OnCombatStart();
    virtual void OnRoundStart();
    virtual void OnRoundEnd();
    virtual void OnEachTurnStart(CharacterBase* destination);
    virtual void OnTurnStart();
    virtual void OnTurnEnd();
    virtual void OnHit();
    virtual void OnDead();
    virtual void OnKill(CharacterBase* destination);
    virtual void OnTokenAdded(int tokenID);
    virtual void OnTokenRemoved(int tokenID);
};
