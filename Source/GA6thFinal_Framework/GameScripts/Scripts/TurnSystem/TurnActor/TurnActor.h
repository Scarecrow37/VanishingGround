#pragma once
#include "UmFramework.h"
class TurnActor : public Component
{
    USING_PROPERTY(TurnActor)
public:
    inline static constexpr const char* TAG = "TurnActor";
    enum class STATE
    {
        Dead,
        Wait,
        Play,
    };

public:
    REFLECT_PROPERTY(
        State,
        RandomSpeed, 
        RoundSpeed
        )

public:
    TurnActor();
    virtual ~TurnActor();

public:
    /// <summary>
    /// TurnActor의 턴을 시작합니다. OnTurnStart를 호출합니다.
    /// </summary>
    virtual void PlayTurn() final;

public:
    virtual int GetSpeed() = 0;

protected:
    virtual void OnTurnStart() = 0;

public:
    GETTER_ONLY(int, RandomSpeed) { return _randomSpeed; }
    PROPERTY(RandomSpeed)
    void SetRandomSpeed(int randomSpeed) 
    { 
        _randomSpeed = randomSpeed; 
        _randomSpeed = std::clamp(_randomSpeed, 0, 6);
    }

    GETTER_ONLY(int, RoundSpeed) 
    { 
        int roundSpeed = GetSpeed() + _randomSpeed;
        return std::clamp(roundSpeed, -99, 99);
    }
    PROPERTY(RoundSpeed)

    //SETTER(STATE, State) { _currState = value; }
    GETTER_ONLY(STATE, State) { return _currState; }
    PROPERTY(State)

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

};
