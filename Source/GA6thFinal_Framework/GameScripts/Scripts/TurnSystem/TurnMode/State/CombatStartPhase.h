#pragma once
#include "Monster/Common/MonsterCommon.h"
#include "Base/TurnModeStateBase.h"

class Enemy;
class Player;
class CharacterBase;

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
    Enemy* GetEnemyFromSpawnPoint(Monster::SpawnPoint spawnPoint) const;
    const std::vector<CharacterBase*>& GetCharacters() { return _characters; }

protected:
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

    void NotifyCombatStart();

    /*사용하는 모든 액션들 라이프 사이클에 등록*/
    void AddValidActions();

    /*스테이지 반복 플레이 디버프 소멸계시 추가*/
    void AddExtinctionRevelation() const;

private:
    bool                        _phaseEnd;
    bool                        _waitPhaseEnd;
    Player*                     _player;
    std::vector<Enemy*>         _enemies;
    std::vector<CharacterBase*> _characters;

    std::map<Monster::SpawnPoint, Enemy*> _spawnPointEnemyTable;

private:
    /*적 배열을 Left, Middle, Right 순으로 정렬합니다.*/
    void RegisterEnemiesHUD();
    void RegisterEnemiesHP() const;
    void RegisterEnemyHP(int point, const std::string& key, const std::string& tag) const;
    void RegisterEnemiesChain();
    void RegisterEnemyChain(int point, const std::string& key, const std::string& tag);
    void ReviveEnemies();
    void ResetPlayer();
    void RefreshUI();
    void PassWorldPositionToHud();
};