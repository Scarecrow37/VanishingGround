#pragma once
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include <Stats/Weapon/WeaponStats.h>

class FiniteStateMachine;
class Player : public CharacterBase
{
    USING_PROPERTY(Player)
    inline static Player* static_instance = nullptr;

public:
    inline static constexpr const char* TAG = "Player";
  

public:
    REFLECT_PROPERTY(
        Shield
    )

    GETTER_ONLY(int, Shield) { return GetShield(); }
    PROPERTY(Shield)

public:
    Player();
    virtual ~Player();

protected:
    REFLECT_FIELDS_BEGIN(CharacterBase)
    REFLECT_FIELDS_END(Player)

private:
    class PlayerStatsComponent* _playerStats = nullptr;
    PlayerStatsComponent* GetPlayerStats();
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

    inline static Player* GetInstance() { return static_instance; }
    FiniteStateMachine& GetFSM() { return *_finiteStateMachine; }
    const PlayerStates& GetFSMStates() { return _fsmStates; }

public:
    // CharacterBase을(를) 통해 상속됨
    virtual int GetSpeed() override;
    virtual int GetRandomSpeed() override;
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
    virtual void SerializedReflectEvent() override;

    /// <summary>
    /// <para> 역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.  </para>
    /// <para> 직접 override 해서 사용합니다.                     </para>
    /// </summary>
    virtual void DeserializedReflectEvent() override;

    /// <summary>
    /// <para>  ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다. </para>
    /// </summary>
    virtual void ImGuiDrawPropertysEvent() override;

public:
    virtual void OnCombatStart() override;
    virtual void OnRoundStart() override;
    virtual void OnRoundEnd() override;
    virtual void OnTurnStart() override;
    virtual void OnTurnEnd() override;
    virtual void OnHit() override;
    virtual void OnDead() override;
    virtual void OnKill(CharacterBase* destination) override;
    virtual void OnTokenAdded(int tokenID) override;
    virtual void OnTokenRemoved(int tokenID) override;
};
