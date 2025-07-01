#pragma once
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

class FiniteStateMachine;
class Player : public CharacterBase, public InputReceiver
{
    USING_PROPERTY(Player)
public:
    inline static constexpr const char* TAG = "Player";

public:
    REFLECT_PROPERTY(
        ManaRegenRate, 
        Shield,

        MoveSpeed, 
        RotSpeed
    )

    GETTER_ONLY(int, ManaRegenRate) 
    { 
        return GetManaRegenRate();
    }
    PROPERTY(ManaRegenRate)

    GETTER_ONLY(int, Shield) { return GetShield(); }
    PROPERTY(Shield)
    
    GETTER(float, MoveSpeed) { return _moveSpeed; }
    SETTER(float, MoveSpeed) { _moveSpeed = value; }
    PROPERTY(MoveSpeed)

    GETTER(float, RotSpeed) { return _rotSpeed; }
    SETTER(float, RotSpeed) { _rotSpeed = value; }
    PROPERTY(RotSpeed)

public:
    Player();
    virtual ~Player();

protected:
    REFLECT_FIELDS_BEGIN(CharacterBase)
    REFLECT_FIELDS_END(Player)

    // 테스트 코드
    float _moveSpeed = 1.f;
    float _rotSpeed = 360.f;

private:
    class PlayerStatsComponent* _playerStats = nullptr;
    PlayerStatsComponent* GetPlayerStats();
    int GetManaRegenRate();
    int GetShield();

private:
    FiniteStateMachine* _finiteStateMachine = nullptr;
    void BuildPlayerFSM();
    struct PlayerStates
    {
        class PlayerWaitTurnState* PlayerWaitTurnState = nullptr;
        class PlayerPlayTurnState* PlayerPlayTurnState = nullptr;
        class PlayerDeadState*     PlayerDeadState     = nullptr;
    } 
    _fsmStates;
public:
    /*플레이어의 턴을 시작합니다.*/
    virtual void PlayTurn() override;
    /*플레이어의 턴을 종료합니다.*/
    virtual void EndTurn() override;
    /*플레이어를 사망 상태로 만듭니다.*/
    virtual void Dead() override;

    FiniteStateMachine& GetFSM() { return *_finiteStateMachine; }
    const PlayerStates& GetFSMStates() { return _fsmStates; }

public:
    // CharacterBase을(를) 통해 상속됨
    int GetSpeed() override;
    CharacterStats* GetCharacterStats() override;

protected:
    /// <summary>
    /// <para> 이 함수는 항상 Start 함수 전에 호출되며 프리팹이 인스턴스화 된 직후에 호출됩니다.                </para>
    /// <para> 게임 오브젝트의 Active가 false 상태인 경우 Awake 함수는 true가 될때까지 호출되지 않습니다.      </para>
    /// </summary>
    virtual void Awake();
    
    /// <summary>
    /// Update 는 프레임당 한 번 호출됩니다.
    /// </summary>
    virtual void Update();

    /// <summary>
    /// <para> 직렬화 직전 자동으로 호출되는 이벤트 함수입니다. </para>
    /// <para> 직접 override 해서 사용합니다.                 </para>
    /// </summary>
    virtual void SerializedReflectEvent() {}

    /// <summary>
    /// <para> 역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.  </para>
    /// <para> 직접 override 해서 사용합니다.                     </para>
    /// </summary>
    virtual void DeserializedReflectEvent() {}

    /// <summary>
    /// <para>  ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다. </para>
    /// </summary>
    virtual void ImGuiDrawPropertysEvent();

    void OnInput(const Input::Controller& controller);
};
