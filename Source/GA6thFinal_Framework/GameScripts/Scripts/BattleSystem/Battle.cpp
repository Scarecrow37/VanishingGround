#include "pchScripts.h"
#include "Battle.h"

#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <WeaponSystem/WeaponSystem.h>
#include <DamageSystem/DamageSystem.h>
#include <Stats/Player/PlayerStats.h>
#include <Stats/Player/PlayerStatsComponent.h>
#include <Stats/Enemy/EnemyStats.h>
#include <Stats/Enemy/EnemyStatsComponent.h>

void Battle::operator()(Player& attacker, EnemyTargetFlag targetFlag)
{
    TurnMode* turnMode = TurnMode::GetInstance();
    if (turnMode)
    {
        turnMode->ApplyActions(
            [&targetFlag](TurnAction& turnAction) { turnAction.OnPlayerBattleTargetSelected(targetFlag); });

        std::vector<Enemy*> targets = GetTargetsFromFlags(targetFlag);
        for (auto& enemy : targets)
        {
            BattleStart(attacker, *enemy);
        }
    }
}

void Battle::operator()(Enemy& attacker, Player& target)
{
    TurnMode* turnMode = TurnMode::GetInstance();
    if (turnMode)
    {
        BattleStart(attacker, target);
    }
}

std::vector<Enemy*> Battle::GetTargetsFromFlags(EnemyTargetFlag targetFlag)
{
    std::vector<Enemy*> selectedTargets;
    TurnMode* turnMode = TurnMode::GetInstance();
    if (turnMode)
    {
        CombatStartPhase* combatStartPhase = turnMode->States->CombatStartPhase;
        if (combatStartPhase)
        {
            bool isValidFlag = (targetFlag & ~ENEMY_TARGET_FLAG_ALL) == 0;
            if (isValidFlag)
            {
                auto& enemys = combatStartPhase->GetEnemies();
                EnemyTargetBitset bitset(targetFlag);
                for (size_t i = 0; i < bitset.size(); i++)
                {
                    if (bitset.test(i))
                    {
                        try
                        {
                            Enemy* enemy = enemys.at(i);
                            if (enemy)
                            {
                                selectedTargets.push_back(enemy);
                            }
                        }
                        catch (const std::exception&)
                        {
                            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"유효하지 않은 enemies 범위입니다.");
                        }
                    }
                }
            }
        }
    }
    return selectedTargets;
}

void Battle::BattleStart(Player& attacker, Enemy& target)
{
    TurnMode*             turnMode             = TurnMode::GetInstance();
    WeaponSystem*         weaponSystem         = WeaponSystem::GetInstance();
    PlayerStatsComponent* playerStatsComponent = attacker.GetPlayerStats();
    EnemyStatsComponent*  enemyStatsComponent  = target.GetEnemyStats();
    if (turnMode && weaponSystem && playerStatsComponent && enemyStatsComponent)
    {
        lastAttacker    = std::static_pointer_cast<CharacterBase>(attacker.GetWeakPtr().lock());
        lastTarget      = std::static_pointer_cast<CharacterBase>(target.GetWeakPtr().lock());
        lastTargetEnemy = std::static_pointer_cast<Enemy>(target.GetWeakPtr().lock());

        PlayerStats playerStats(playerStatsComponent->GetStats());
        WeaponStats weaponStats(weaponSystem->GetCurrentWeaponStats());
        EnemyStats  enemyStats(enemyStatsComponent->GetStats());

        PlayerInfo playerInfo(attacker, weaponStats, playerStats);
        EnemyInfo  enemyInfo(target, enemyStats);

        turnMode->ApplyActions([&](TurnAction& action) {
            action.OnPlayerBattleCaculateChainModifier(attacker, playerStats, weaponStats, target, enemyStats);
        });
        int chainDamage = DamageSystem::CalculateChainDamage(playerInfo, enemyInfo);
        target.TakeChain(chainDamage);

        turnMode->ApplyActions([&](TurnAction& action) {
            action.OnPlayerBattleCalculateDamageModifier(attacker, playerStats, weaponStats, target, enemyStats);
        });
        int damage = DamageSystem::CalculateDamage(playerInfo, enemyInfo);
        target.TakeDamage(damage);
    }
}

void Battle::BattleStart(Enemy& attacker, Player& target) 
{
    TurnMode*             turnMode             = TurnMode::GetInstance();
    WeaponSystem*         weaponSystem         = WeaponSystem::GetInstance();
    EnemyStatsComponent*  enemyStatsComponent  = attacker.GetEnemyStats();
    PlayerStatsComponent* playerStatsComponent = target.GetPlayerStats();
    if (turnMode && weaponSystem && playerStatsComponent && enemyStatsComponent)
    {
        lastAttacker = std::static_pointer_cast<CharacterBase>(attacker.GetWeakPtr().lock());
        lastTarget   = std::static_pointer_cast<CharacterBase>(target.GetWeakPtr().lock());

        EnemyStats  enemyStats(enemyStatsComponent->GetStats());
        PlayerStats playerStats(playerStatsComponent->GetStats());
        EnemyInfo  enemyInfo(attacker, enemyStats);
        PlayerInfo playerInfo(target, weaponSystem->GetCurrentWeaponStats(), playerStats);

        int chainDamage = DamageSystem::CalculateChainDamage(enemyInfo, playerInfo);
        turnMode->ApplyActions([&](TurnAction& action) {
            action.OnEnemyBattleCalculateChainModifier(attacker, enemyStats, target, playerStats);
        });
        target.TakeChain(chainDamage);

        turnMode->ApplyActions(
            [&](TurnAction& action) { action.OnEnemyBattleCalculateDamageModifier(attacker, enemyStats, target, playerStats); });
        int damage = DamageSystem::CalculateDamage(enemyInfo, playerInfo);
        target.TakeDamage(damage);
    }
}
