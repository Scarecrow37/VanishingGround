#pragma once
#include "Base/TurnModeStateBase.h"

class Enemy;
class Player;

/*
* 전투 시작 페이즈 입니다.
* 전투 시작시 연출 및 초기화를 담당합니다.
*/
class CombatStartPhase : public TurnModeStateBase
{
public:
    CombatStartPhase();
    virtual ~CombatStartPhase() override;

    /// <summary>
    /// 페이즈 종료 여부를 반환합니다.
    /// </summary>
    /// <returns></returns>
    bool IsEndPhase() const { return _phaseEnd; }
    
public:
    /// <summary>
    /// 캐릭터들의 스텟을 초기화합니다.
    /// </summary> 
    void ResetCharacterStats();
    Player* GetPlayer() { return _player; }
    const std::vector<Enemy*>& GetEnemies() { return _enemies; }

protected:
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

private:
    bool                _phaseEnd;
    Player*             _player;
    std::vector<Enemy*> _enemies;
};