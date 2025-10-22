#pragma once
#include "../CharacterBase.h"
#include "Enum/EnemyEnum.h"
#include "AI/EnemyAI.h"

class ParticleComponent;
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
    virtual void Revive() override;

private:
    EnemyAI _aiModel;
    EnemyStatsComponent* _enemyStats = nullptr;
    ParticleComponent*   _hitParticle = nullptr; // 피격 이펙트 파티클

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
    virtual void TakeDamage(int damage, const QTE::NoteResult& result, bool playAnim = true) override;

    inline EnemyAI&            GetAIModel() { return _aiModel; }
    inline FiniteStateMachine& GetFSM() { return *_finiteStateMachine; }
    inline const EnemyStates&  GetFSMStates() { return _fsmStates; }

    /*Enemy의 Stats을 반환합니다.*/
    EnemyStatsComponent* GetEnemyStats();

public:
    GameObject* GetMonsterHUD() const { return _monsterHUD; }
    void SetMonsterHUD(GameObject* HUD);

private:
    GameObject* _monsterHUD = nullptr;

protected:
    virtual void Awake();
    virtual void Update();

    virtual void PlayTurn() override;
    CharacterStats* GetCharacterStats() override;

    void InitParticle();

private:
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
    void OnNotifiedAnimationEvent(const Timeline::EventContext* context) override;
};
