#pragma once
#include "../CharacterBase.h"
#include "Enum/EnemyEnum.h"
#include "AI/EnemyAI.h"

class EnemyStatsComponent;
class FSMState;

class Enemy : public CharacterBase
{
    USING_PROPERTY(Enemy)
public:
    inline static constexpr const char* TAG = "Enemy";

public:
    REFLECT_PROPERTY(
        Speed, Type
        )

    GETTER_ONLY(int, Speed) { return GetSpeed(); }
    PROPERTY(Speed)

    SETTER(EnemyType, Type) { ReflectFields->Type = value; }
    GETTER(EnemyType, Type) { return ReflectFields->Type; }
    PROPERTY(Type)

public:
    Enemy();
    virtual ~Enemy();

protected:
    REFLECT_FIELDS_BEGIN(CharacterBase)
    EnemyType Type = EnemyType::MONSTER_A;
    REFLECT_FIELDS_END(Enemy)

public:
    virtual int GetSpeed() override;

private:
    EnemyAI _aiModel;
    EnemyStatsComponent* _enemyStats = nullptr;

protected:
    class FiniteStateMachine* _finiteStateMachine = nullptr;

    void BuildEnemyFSM();
    struct EnemyStates
    {
        class EnemyWaitTurnState* WaitTurn = nullptr;   // 턴 종료 상태
        class EnemyPlayTurnState* PlayTurn = nullptr; // 턴 시작 상태
        class EnemyDeadState*     Dead     = nullptr;   // 사망 상태
    } 
    _fsmStates;

public:
    /*Enemy의 턴을 종료합니다.*/
    virtual void EndTurn() override;
    /*Enemy를 Dead 상태로 만듭니다.*/
    virtual void Dead() override;
    /*Enemy에게 피격을 가합니다.*/
    virtual void TakeDamage(int damage, bool playAnim = true) override;

    inline EnemyAI&            GetAIModel() { return _aiModel; }
    inline FiniteStateMachine& GetFSM() { return *_finiteStateMachine; }
    inline const EnemyStates&  GetFSMStates() { return _fsmStates; }

    /*Enemy의 Stats을 반환합니다.*/
    EnemyStatsComponent* GetEnemyStats();

protected:
    /// <summary>
    /// <para> 이 함수는 항상 Start 함수 전에 호출되며 프리팹이 인스턴스화 된 직후에 호출됩니다.                </para>
    /// <para> 게임 오브젝트의 Active가 false 상태인 경우 Awake 함수는 true가 될때까지 호출되지 않습니다.      </para>
    /// </summary>
    virtual void Awake();

    virtual void Update();

    virtual void Revive() override;
    virtual void PlayTurn() override;
    CharacterStats* GetCharacterStats() override;

private:
    virtual void OnCombatStart() override;
    virtual void OnRoundStart() override;
    virtual void OnRoundEnd() override;
    virtual void OnEachTurnStart(CharacterBase* destination) override;
    virtual void OnTurnStart() override;
    virtual void OnTurnEnd() override;
    virtual void OnHit() override;
    virtual void OnKill(CharacterBase* destination) override;
    virtual void OnTokenAdded(int tokenID) override;
    virtual void OnTokenRemoved(int tokenID) override;
    virtual void OnNotifiedAnimationEvent(const Timeline::EventContext* context) override;
};
