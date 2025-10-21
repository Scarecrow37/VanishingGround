#pragma once
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "Stats/Weapon/WeaponStats.h"
#include "Utility/SingletonHelper.h"

class PlayerStatsComponent;
class FiniteStateMachine;
class Player : public CharacterBase
{
    USING_PROPERTY(Player)

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
    SingletonComponent<Player> _singletonComponent{this};

    class PlayerStatsComponent* _playerStats = nullptr;
    int GetShield();

private:
    FiniteStateMachine* _finiteStateMachine = nullptr;
    void BuildPlayerFSM();
    struct PlayerStates
    {
        class PlayerWaitTurnState* PlayerWaitTurnState = nullptr;
        class PlayerPlayTurnState* PlayerPlayTurnState = nullptr;
        class PlayerDeadState*     PlayerDeadState     = nullptr;
        class PlayerWinState*      PlayerWinState      = nullptr;
    } 
    _fsmStates;
public:
    /*플레이어의 턴을 시작합니다.*/
    virtual void PlayTurn() override;
    /*플레이어의 턴을 종료합니다.*/
    virtual void EndTurn() override;
    /*플레이어를 사망 상태로 만듭니다.*/
    virtual void Dead() override;
    /*플레이어에게 피격을 가합니다.*/
    virtual void TakeDamage(int damage, bool playAnim = true) override;
    virtual void TakeDamage(int damage, const QTE::NoteResult& result, bool playAnim = true) override;

    FiniteStateMachine& GetFSM() { return *_finiteStateMachine; }
    const PlayerStates& GetFSMStates() { return _fsmStates; }

public:
    virtual int GetSpeed() override;
    virtual int GetRandomSpeed() override;
    CharacterStats* GetCharacterStats() override;

    /// <summary>
    /// 플레이어 스텟 컴포넌트를 반환합니다. 존재하지 않으면 nullptr 입니다.
    /// </summary>
    /// <returns></returns>
    PlayerStatsComponent* GetPlayerStats();

protected:
    void Awake();
    void Update();

    /// <summary>
    /// <para> 직렬화 직전 자동으로 호출되는 이벤트 함수입니다. </para>
    /// <para> 직접 override 해서 사용합니다.                 </para>
    /// </summary>
    void SerializedReflectEvent() override;

    /// <summary>
    /// <para> 역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.  </para>
    /// <para> 직접 override 해서 사용합니다.                     </para>
    /// </summary>
    void DeserializedReflectEvent() override;

    /// <summary>
    /// <para>  ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다. </para>
    /// </summary>
    void ImGuiDrawPropertysEvent() override;

public:
    void OnCombatStart() override;
    void OnRoundStart() override;
    void OnRoundEnd() override;
    void OnEachTurnStart(CharacterBase* destination) override;
    void OnTurnStart() override;
    void OnTurnEnd() override;
    void OnHit() override;
    void OnKill(CharacterBase* destination) override;
    void OnTokenAdded(int tokenID) override;
    void OnTokenRemoved(int tokenID) override;
    void OnQTEStart() override;
    void OnQTEEnd() override;
    void OnNotifiedAnimationEvent(const Timeline::EventContext* context) override;
};
