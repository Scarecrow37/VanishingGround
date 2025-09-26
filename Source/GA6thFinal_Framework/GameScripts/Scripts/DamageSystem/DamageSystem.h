#pragma once
#include "UmFrameWork.h"
#include <QTE/Result/QTEResult.h>

class Enemy;
struct EnemyStats;
class Player;
struct PlayerStats;
struct WeaponStats;

struct PlayerInfo
{
    friend class DamageSystem;
    PlayerInfo(const Player& player, const WeaponStats& weaponStats, const PlayerStats& playerStats)
        : _player(player), _weaponStats(weaponStats), _playerStats(playerStats)
    {
    }
    ~PlayerInfo() = default;

private:
    const Player&      _player;
    const WeaponStats& _weaponStats;
    const PlayerStats& _playerStats;
};

struct EnemyInfo
{
    friend class DamageSystem;
    EnemyInfo(const Enemy& enemy, const EnemyStats& enemyStats) 
        : _enemy(enemy), _enemyStats(enemyStats) 
    {
    }
    ~EnemyInfo() = default;

private:
    const Enemy&      _enemy;
    const EnemyStats& _enemyStats;
    const int         _enemyActionID = 0;
};

class DamageSystem : public Component
{
public:
    static int CalculateDamage(const PlayerInfo& attacker, const EnemyInfo& target, const QTE::NoteResult& result);
    static int CalculateChainDamage(const PlayerInfo& attacker, const EnemyInfo& target);

    static int CalculateDamage(const EnemyInfo& attacker, const PlayerInfo& target);
    static int CalculateChainDamage(const EnemyInfo& attacker, const PlayerInfo& target);
};