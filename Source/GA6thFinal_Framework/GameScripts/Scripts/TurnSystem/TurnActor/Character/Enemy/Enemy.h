#pragma once
#include "../CharacterBase.h"

class EnemyStatsComponent;
class Enemy : public CharacterBase
{
    USING_PROPERTY(Enemy)
public:
    inline static constexpr const char* TAG = "Enemy";

public:
    REFLECT_PROPERTY(
        Speed
        )

    GETTER_ONLY(int, Speed) { return GetSpeed(); }
    PROPERTY(Speed)
public:
    Enemy();
    virtual ~Enemy();

protected:
    REFLECT_FIELDS_BEGIN(CharacterBase)
    REFLECT_FIELDS_END(Enemy)

public:
    int GetSpeed() override;

private:
    EnemyStatsComponent* _enemyStats = nullptr;
    EnemyStatsComponent* GetEnemyStats();

protected:
    class FiniteStateMachine* _finiteStateMachine = nullptr;
    void BuildEnemyFSM();
    struct EnemyStates
    {
        
    } 
    _fsmStates;

public:
    /*Enemy의 턴을 종료합니다.*/
    virtual void EndTurn() override;

    /*Enemy를 Dead 상태로 만듭니다.*/
    virtual void Dead() override;

    FiniteStateMachine& GetFSM() { return *_finiteStateMachine; }
    const EnemyStates&  GetFSMStates() { return _fsmStates; }

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
};
