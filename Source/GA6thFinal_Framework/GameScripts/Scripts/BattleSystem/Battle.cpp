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
#include <Monster/Common/MonsterCommon.h>
#include <Monster/System/MonsterSystem.h>

void Battle::operator()(Player& attacker, EnemyTargetFlag targetFlag, QTE::NoteResult& result)
{
    TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance();
    if (turnMode)
    {
        //연격은 최우선적으로 계산
        currentChainDamageSet.clear();
        std::vector<Enemy*> chainTargets = GetTargetsFromFlags(targetFlag);
        for (auto& enemy : chainTargets)
        {
            ChainStart(attacker, *enemy, result);         
        }
        turnMode->ApplyActions([&targetFlag](TurnAction& turnAction) { turnAction.OnPlayerBattleTargetSelected(targetFlag); });
        std::vector<Enemy*> targets = GetTargetsFromFlags(targetFlag);
        //공격 대상이 달라질 수 있기 때문에 연격을 다시 계산 (중복 계산은 안일어남)
        for (auto& enemy : targets)
        {
            ChainStart(attacker, *enemy, result);       
        }
        //데미지 계산
        for (auto& enemy : targets)
        {
            BattleStart(attacker, *enemy, result);
        }
    }
}

void Battle::operator()(Enemy& attacker, Player& target)
{
    TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance();
    if (turnMode)
    {
        ChainStart(attacker, target);  //연격 계산
        BattleStart(attacker, target); //데미지 계산
    }
}

std::vector<Enemy*> Battle::GetTargetsFromFlags(EnemyTargetFlag targetFlag)
{
    std::vector<Enemy*> selectedTargets;
    if (MonsterSystem* monsterSystem = SingletonComponent<MonsterSystem>::GetInstance())
    {
        EnemyTargetBitset bitset(targetFlag);
        for (size_t i = 0; i < bitset.size(); i++)
        {
            if (bitset.test(i))
            {
                auto weakEnemy = monsterSystem->GetSpawnedEnemyFromSpawnPoint(static_cast<Monster::SpawnPoint>(i));
                if (Enemy* enemy = weakEnemy.lock().get())
                {
                    selectedTargets.push_back(enemy);
                }
            }
        }
    }
    return selectedTargets;
}

void Battle::ChainStart(Player& attacker, Enemy& target, QTE::NoteResult& result)
{
    auto [iter, insertResult] = currentChainDamageSet.insert(&target); // 이번턴 연격 계산된 적들 중복 체크
    if (insertResult)
    {
        if (result.IsHit())
        {
            TurnMode*             turnMode             = SingletonComponent<TurnMode>::GetInstance();
            WeaponSystem*         weaponSystem         = SingletonComponent<WeaponSystem>::GetInstance();
            PlayerStatsComponent* playerStatsComponent = attacker.GetPlayerStats();
            EnemyStatsComponent*  enemyStatsComponent  = target.GetEnemyStats();
            if (turnMode && weaponSystem && playerStatsComponent && enemyStatsComponent)
            {
                lastAttacker    = std::static_pointer_cast<CharacterBase>(attacker.GetWeakPtr().lock());
                lastTarget      = std::static_pointer_cast<CharacterBase>(target.GetWeakPtr().lock());
                lastTargetEnemy = std::static_pointer_cast<Enemy>(target.GetWeakPtr().lock());

                PlayerStats playerStats(playerStatsComponent->GetStats());
                WeaponStats weaponStats(weaponSystem->GetCurrentWeaponElement().Stats);
                EnemyStats  enemyStats(enemyStatsComponent->GetStats());

                PlayerInfo playerInfo(attacker, weaponStats, playerStats);
                EnemyInfo  enemyInfo(target, enemyStats);

                // 토큰용 데이터
                PlayerAttackData attackerData = {
                    .Source = attacker, .SourceStats = playerStats, .WeaponStats = weaponStats, .NoteResult = result};
                EnemyHitData targetData = {.Source = target, .SourceStats = enemyStats};

                attacker.GetTokenInventory().NotifyPrePlayerAttackCalculateChain(attackerData, targetData);
                target.GetTokenInventory().NotifyPreEnemyHitCalculateChain(attackerData, targetData);

                int chainDamage = 0;
                turnMode->ApplyActions([&](TurnAction& action) {
                    action.OnPlayerBattleCalculateChainModifier(attacker, playerStats, weaponStats, target, enemyStats);
                });
                chainDamage = DamageSystem::CalculateChainDamage(playerInfo, enemyInfo);

                attacker.GetTokenInventory().NotifyPostPlayerAttackCalculateChain(attackerData, targetData, chainDamage);
                target.GetTokenInventory().NotifyPostEnemyHitCalculateChain(attackerData, targetData, chainDamage);
                target.TakeChain(chainDamage);
            }
        }
    }
}

void Battle::BattleStart(Player& attacker, Enemy& target, QTE::NoteResult& result)
{
    TurnMode*             turnMode             = SingletonComponent<TurnMode>::GetInstance();
    WeaponSystem*         weaponSystem         = SingletonComponent<WeaponSystem>::GetInstance();
    PlayerStatsComponent* playerStatsComponent = attacker.GetPlayerStats();
    EnemyStatsComponent*  enemyStatsComponent  = target.GetEnemyStats();
    if (turnMode && weaponSystem && playerStatsComponent && enemyStatsComponent)
    {
        lastAttacker    = std::static_pointer_cast<CharacterBase>(attacker.GetWeakPtr().lock());
        lastTarget      = std::static_pointer_cast<CharacterBase>(target.GetWeakPtr().lock());
        lastTargetEnemy = std::static_pointer_cast<Enemy>(target.GetWeakPtr().lock());

        PlayerStats playerStats(playerStatsComponent->GetStats());
        WeaponStats weaponStats(weaponSystem->GetCurrentWeaponElement().Stats);
        EnemyStats  enemyStats(enemyStatsComponent->GetStats());

        PlayerInfo playerInfo(attacker, weaponStats, playerStats);
        EnemyInfo  enemyInfo(target, enemyStats);

        int damage = 0;

        // 토큰용 데이터
        PlayerAttackData attackerData = {
            .Source = attacker, .SourceStats = playerStats, .WeaponStats = weaponStats, .NoteResult = result};
        EnemyHitData targetData = {.Source = target, .SourceStats = enemyStats};
        
        attacker.GetTokenInventory().NotifyPrePlayerAttackCalculateDamage(attackerData, targetData);
        target.GetTokenInventory().NotifyPreEnemyHitCalculateDamage(attackerData, targetData);

        turnMode->ApplyActions([&](TurnAction& action) {
            action.OnPlayerBattlePreCalculate(attacker, playerStats, weaponStats, target, enemyStats, result);
        });

        if (result.IsHit())
        {
            turnMode->ApplyActions([&](TurnAction& action) {
                action.OnPlayerBattleCalculateDamageModifier(attacker, playerStats, weaponStats, target, enemyStats);
            });
            damage = DamageSystem::CalculateDamage(playerInfo, enemyInfo, result);
        }

        attacker.GetTokenInventory().NotifyPostPlayerAttackCalculateDamage(attackerData, targetData, damage);
        target.GetTokenInventory().NotifyPostEnemyHitCalculateDamage(attackerData, targetData, damage);
        // 미스여도 TakeDamage를 호출. 어차피 내부에서 미스처리를 하기 때문 (판정에 따른 이펙트 출력때문에... 나중에 PlayEffect를 따로 만들까? 싶음)
        target.TakeDamage(damage, result);
    }
}

void Battle::ChainStart(Enemy& attacker, Player& target)
{
    TurnMode*             turnMode             = SingletonComponent<TurnMode>::GetInstance();
    WeaponSystem*         weaponSystem         = SingletonComponent<WeaponSystem>::GetInstance();
    EnemyStatsComponent*  enemyStatsComponent  = attacker.GetEnemyStats();
    PlayerStatsComponent* playerStatsComponent = target.GetPlayerStats();
    if (turnMode && weaponSystem && playerStatsComponent && enemyStatsComponent)
    {
        lastAttacker = std::static_pointer_cast<CharacterBase>(attacker.GetWeakPtr().lock());
        lastTarget   = std::static_pointer_cast<CharacterBase>(target.GetWeakPtr().lock());

        EnemyStats  enemyStats(enemyStatsComponent->GetStats());
        PlayerStats playerStats(playerStatsComponent->GetStats());
        EnemyInfo   enemyInfo(attacker, enemyStats);
        PlayerInfo  playerInfo(target, weaponSystem->GetCurrentWeaponElement().Stats, playerStats);

        // 토큰용 데이터
        EnemyAttackData attackerData = {.Source = attacker, .SourceStats = enemyStats};
        PlayerHitData   targetData   = {.Source = target, .SourceStats = playerStats};

        attacker.GetTokenInventory().NotifyPreEnemyAttackCalculateChain(attackerData, targetData);
        target.GetTokenInventory().NotifyPrePlayerHitCalculateChain(attackerData, targetData);

        int chainDamage = DamageSystem::CalculateChainDamage(enemyInfo, playerInfo);
        turnMode->ApplyActions([&](TurnAction& action) {
            action.OnEnemyBattleCalculateChainModifier(attacker, enemyStats, target, playerStats);
        });

        attacker.GetTokenInventory().NotifyPostEnemyAttackCalculateChain(attackerData, targetData, chainDamage);
        target.GetTokenInventory().NotifyPostPlayerHitCalculateChain(attackerData, targetData, chainDamage);
        target.TakeChain(chainDamage);
    }
}


void Battle::BattleStart(Enemy& attacker, Player& target)
{
    TurnMode*             turnMode             = SingletonComponent<TurnMode>::GetInstance();
    WeaponSystem*         weaponSystem         = SingletonComponent<WeaponSystem>::GetInstance();
    EnemyStatsComponent*  enemyStatsComponent  = attacker.GetEnemyStats();
    PlayerStatsComponent* playerStatsComponent = target.GetPlayerStats();
    if (turnMode && weaponSystem && playerStatsComponent && enemyStatsComponent)
    {
        lastAttacker = std::static_pointer_cast<CharacterBase>(attacker.GetWeakPtr().lock());
        lastTarget   = std::static_pointer_cast<CharacterBase>(target.GetWeakPtr().lock());

        EnemyStats  enemyStats(enemyStatsComponent->GetStats());
        PlayerStats playerStats(playerStatsComponent->GetStats());
        EnemyInfo  enemyInfo(attacker, enemyStats);
        PlayerInfo playerInfo(target, weaponSystem->GetCurrentWeaponElement().Stats, playerStats);

        // 토큰용 데이터
        EnemyAttackData attackerData = {.Source = attacker, .SourceStats = enemyStats};
        PlayerHitData   targetData   = {.Source = target, .SourceStats = playerStats};

        attacker.GetTokenInventory().NotifyPreEnemyAttackCalculateDamage(attackerData, targetData);
        target.GetTokenInventory().NotifyPrePlayerHitCalculateDamage(attackerData, targetData);

        turnMode->ApplyActions(
            [&](TurnAction& action) { action.OnEnemyBattleCalculateDamageModifier(attacker, enemyStats, target, playerStats); });
        int damage = DamageSystem::CalculateDamage(enemyInfo, playerInfo);

        attacker.GetTokenInventory().NotifyPostEnemyAttackCalculateDamage(attackerData, targetData, damage);
        target.GetTokenInventory().NotifyPostPlayerHitCalculateDamage(attackerData, targetData, damage);

        target.TakeDamage(damage);
    }
}