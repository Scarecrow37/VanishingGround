#pragma once
#include "../CharacterBase.h"
#include "Enum/EnemyEnum.h"
#include "AI/EnemyAI.h"

#include "Monster/Common/MonsterCommon.h"
#include "Monster/Controller/MonsterController.h"

class ParticleComponent;
class EnemyStatsComponent;
class FSMState;

class Enemy : public CharacterBase
{
    USING_PROPERTY(Enemy)
public:
    inline static constexpr const char* TAG = "Enemy";

public:
    REFLECT_PROPERTY(RandomSpeed, Speed, Type, SpawnPoint)

    // OnRoundStart 진입시 자동으로 랜덤한 값이 부여됩니다.
    GETTER_ONLY(int, RandomSpeed) { return GetRandomSpeed(); }
    PROPERTY(RandomSpeed)

    GETTER_ONLY(int, Speed) { return GetSpeed(); }
    PROPERTY(Speed)

    SETTER(EnemyType, Type) { ReflectFields->Type = value; }
    GETTER(EnemyType, Type) { return ReflectFields->Type; }
    PROPERTY(Type)

    GETTER_ONLY(Monster::SpawnPoint, SpawnPoint) { return _spawnPoint; }
    PROPERTY(SpawnPoint)

public:
    Enemy();
    virtual ~Enemy();

protected:
    REFLECT_FIELDS_BEGIN(CharacterBase)
    EnemyType Type = EnemyType::MONSTER_A;
    REFLECT_FIELDS_END(Enemy)

private:
    Monster::SpawnPoint  _spawnPoint = Monster::SpawnPoint::Invalid;
    Monster::Controller  _controller;
    EnemyStatsComponent* _enemyStats = nullptr;

protected:
    class FiniteStateMachine* _finiteStateMachine = nullptr;

    void BuildEnemyFSM();
    struct EnemyStates
    {
        class EnemyWaitTurnState* WaitTurn = nullptr;   // 턴 종료 상태
        class EnemyPlayTurnState* PlayTurn = nullptr;   // 턴 시작 상태
        class EnemyDeadState*     Dead     = nullptr;   // 사망 상태
    } 
    _fsmStates;

    int _randomSpeed = 0;

public:
    /*Enemy의 턴을 종료합니다.*/
    void EndTurn() override;
    /*Enemy를 Dead 상태로 만듭니다.*/
    void Dead() override;
    /**/
    void Revive() override;
    /*Enemy에게 피격을 가합니다.*/
    void TakeDamage(int damage, bool playAnim = true) override;
    void TakeDamage(int damage, const QTE::NoteResult& result, bool playAnim = true);
    void ShowDamage(int damage, std::span<std::string> sources) override;
    void ShowCriticalDamage(int damage, std::span<std::string> sources);
    void Heal(int amount) override;
    void ShowHeal(int healAmount, std::span<std::string> sources) override;

    inline Monster::Controller&     GetController() { return _controller; }
    inline FiniteStateMachine&      GetFSM() { return *_finiteStateMachine; }
    inline const EnemyStates&       GetFSMStates() { return _fsmStates; }

    /*Enemy의 Stats을 반환합니다.*/
    EnemyStatsComponent* GetEnemyStats();
    CharacterStats* GetCharacterStats() override;

    int GetSpeed() override;
    int GetRandomSpeed() override;

    void SetPositionFromSpawnPoint(Monster::SpawnPoint spawnPoint);

public:
    GameObject* GetMonsterHUD() const { return _monsterHUD; }
    void SetMonsterHUD(GameObject* HUD);

private:
    GameObject* _monsterHUD = nullptr;
    bool        _isCriticalDamage = false;

protected:
    void Awake() override;
    void Update() override;
    void PlayTurn() override;
    void ImGuiDrawPropertysEvent() override;


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
