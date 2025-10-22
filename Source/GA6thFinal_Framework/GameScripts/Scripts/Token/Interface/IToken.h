#pragma once
#include <Token/Enums/TokenEnums.h>
#include <Interface/ITriggerType.h>

class CharacterBase;
class Player;
class Enemy;
struct CharacterStats;
struct PlayerStats;
struct EnemyStats;
struct WeaponStats;

namespace QTE
{
    struct NoteResult;
}

class IToken
{
public:
    IToken();
    virtual ~IToken();

private: // ITriggerType을(를) 통해 상속됨.
    /// <summary>전투가 시작될 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnCombatStart(CharacterBase* source) = 0;

    /// <summary>라운드가 시작될 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnRoundStart(CharacterBase* source) = 0;

    /// <summary>라운드가 끝날 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnRoundEnd(CharacterBase* source) = 0;

    /// <summary>존재하는 각 CharacterBase의 턴이 시작될 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    /// <param name="destination">턴이 시작된 대상</param>
    virtual void OnEachTurnStart(CharacterBase* source, CharacterBase* destination) = 0;

    /// <summary>턴이 시작될 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnTurnStart(CharacterBase* source) = 0;

    /// <summary>턴이 끝날 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnTurnEnd(CharacterBase* source) = 0;

    /// <summary>객체가 Hit 당했을 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnHit(CharacterBase* source) = 0;

    /// <summary>객체가 사망했을 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnDead(CharacterBase* source) = 0;

    /// <summary>객체가 대상을 처치 시 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    /// <param name="destination">처치된 대상</param>
    virtual void OnKill(CharacterBase* source, CharacterBase* destination) = 0;

    /// <summary>객체가 토큰을 얻었을 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnTokenAdded(CharacterBase* source, int tokenID) = 0;

    /// <summary>객체가 토큰이 제거되었을 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnTokenRemoved(CharacterBase* source, int tokenID) = 0;

    /// <summary>객체의 QTE가 시작할 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnQTEStart(CharacterBase* source) = 0;

    /// <summary>객체의 QTE가 끝날 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnQTEEnd(CharacterBase* source) = 0;

    virtual void OnPreBattleCalculateChain(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats,
                                           QTE::NoteResult& noteResult, Enemy& target, EnemyStats& targetStats) = 0;
    virtual void OnPreBattleCalculateChain(Enemy& attacker, EnemyStats& attackerStats, Player& target,
                                           PlayerStats& targetStats)                            = 0;
    virtual void OnPreAttackBattleCalculateDamage(Player& attacker, PlayerStats& attackerStats,
                                                  WeaponStats& weaponStats, QTE::NoteResult& noteResult,
                                                  Enemy& target, EnemyStats& targetStats)       = 0;
    virtual void OnPreAttackBattleCalculateDamage(Enemy& attacker, EnemyStats& attackerStats, Player& target,
                                                  PlayerStats& targetStats)                     = 0;
    virtual void OnPreHitBattleCalculateDamage(Player& attacker, PlayerStats& attackerStats, Enemy& target,
                                               EnemyStats& targetStats)                         = 0;
    virtual void OnPreHitBattleCalculateDamage(Enemy& attacker, EnemyStats& attackerStats, Player& target,
                                               PlayerStats& targetStats)                        = 0;

    virtual void OnTakeDamage(CharacterBase* source, CharacterBase* dest, int& damage, QTE::NoteResult* qteResult) = 0;

    virtual void OnRollRandomSpeed(CharacterBase* source, int& speed) = 0;

public:
    virtual bool                CanAdd(CharacterBase* owner) const      = 0;
    virtual bool                CanRemove(CharacterBase* owner) const   = 0;

    virtual int                 GetTokenOrder() const                   = 0;
    virtual int                 GetTokenID() const                      = 0;
    virtual int                 GetMaxStackCount() const                = 0;
    virtual const std::string&  GetTokenName() const                    = 0;
    virtual const std::string&  GetTokenTag() const                     = 0;
};