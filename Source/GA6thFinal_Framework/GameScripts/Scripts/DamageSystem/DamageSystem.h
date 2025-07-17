#pragma once
#include "UmFrameWork.h"
class Enemy;
struct EnemyStats;
class Player;
struct PlayerStats;
struct WeaponStats;

class DamageSystem : public Component
{
public:
    static int CalculateDamage(const PlayerStats& attacker, const WeaponStats& attackerWeapon, const EnemyStats& target);
};