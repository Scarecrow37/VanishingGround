#pragma once
#include <QTE/Result/QTEResult.h>

class CharacterBase;
class Player;
class Enemy;
struct PlayerStats;
struct WeaponStats;
struct EnemyStats;

/// <summary>
/// 전투를 실행합니다.
/// </summary>
struct Battle
{
public:
    /// <summary>
    /// 타겟을 지정하는 플래그입니다.
    /// </summary>
    enum EnemyTargetFlag_
    {
        ENEMY_TARGET_FLAG_LEFT   = 0b001,
        ENEMY_TARGET_FLAG_MIDDLE = 0b010,
        ENEMY_TARGET_FLAG_RIGHT  = 0b100,
        ENEMY_TARGET_FLAG_ALL    = ENEMY_TARGET_FLAG_LEFT | ENEMY_TARGET_FLAG_MIDDLE | ENEMY_TARGET_FLAG_RIGHT
    };
    using EnemyTargetFlag = int;
    using EnemyTargetBitset = std::bitset<3>;
    //모든 플래그를 순회하기 위한 string_view, flag 값 배열입니다.
    inline constexpr static std::pair<const char*, EnemyTargetFlag_> ENEMY_TARGET_FLAGS[]
    {
        {"LEFT", ENEMY_TARGET_FLAG_LEFT},
        {"MIDDLE", ENEMY_TARGET_FLAG_MIDDLE},
        {"RIGHT", ENEMY_TARGET_FLAG_RIGHT}
    };

    inline constexpr static std::string_view EnemyTargetFlagToString(EnemyTargetFlag_ flag)
    {
        int buttonIndex = std::countr_zero((unsigned int)flag);
        return ENEMY_TARGET_FLAGS[buttonIndex].first;
    }

    static std::vector<Enemy*> GetTargetsFromFlags(EnemyTargetFlag targetFlag);
    
    /// <summary>
    /// 플레이어로 공격을 수행합니다.
    /// </summary>
    /// <param name="attacker :">공격자</param>
    /// <param name="target :">대상</param>
    void operator()(Player& attacker, EnemyTargetFlag targetFlag, QTE::NoteResult& result);

    /// <summary>
    /// 적으로 공격을 수행합니다.
    /// </summary>
    /// <param name="attacker :">공격자</param>
    /// <param name="target :">대상</param>
    void operator()(Enemy& attacker, Player& target);

    /// <summary>
    /// 마지막으로 공격한 CharacterBase를 반환합니다.
    /// </summary>
    /// <returns></returns>
    static const std::weak_ptr<CharacterBase>& GetLastAttacker() { return lastAttacker; }

    /// <summary>
    /// 마지막으로 공격당한 CharacterBase를 반환합니다
    /// </summary>
    /// <returns></returns>
    static const std::weak_ptr<CharacterBase>& GetLastTarget() { return lastTarget; }

    /// <summary>
    /// 마지막으로 공격당한 적을 반환합니다
    /// </summary>
    /// <returns></returns>
    static const std::weak_ptr<Enemy>& GetLastTargetEnemy() { return lastTargetEnemy; }

    /// <summary>
    /// 마지막으로 공격당한 적을 기록하는 변수를 초기화합니다. 전투 시작시 초기화됩니다.
    /// </summary>
    inline static void ResetLastCharacter()
    {
        lastAttacker    = std::weak_ptr<CharacterBase>();
        lastTarget      = std::weak_ptr<CharacterBase>();
        lastTargetEnemy = std::weak_ptr<Enemy>();
    }
   
private:
    inline static std::weak_ptr<CharacterBase> lastAttacker;
    inline static std::weak_ptr<CharacterBase> lastTarget;
    inline static std::weak_ptr<Enemy>         lastTargetEnemy;

    // 이번 턴 연격 계산 여부 (중복 계산 방지)
    inline static std::unordered_set<Enemy*> currentChainDamageSet; 

private:
    static void BattleStart(Player& attacker, Enemy& target, QTE::NoteResult& result);
    static void BattleStart(Enemy& attacker, Player& target);

    static void ChainStart(Player& attacker, Enemy& target, QTE::NoteResult& result);
    static void ChainStart(Enemy& attacker, Player& target);
};

inline Battle::EnemyTargetFlag_ operator|(Battle::EnemyTargetFlag_ lhs, Battle::EnemyTargetFlag_ rhs)
{
    return static_cast<Battle::EnemyTargetFlag_>(static_cast<int>(lhs) | static_cast<int>(rhs));
}