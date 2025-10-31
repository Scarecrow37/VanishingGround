#pragma once
#include <Token/Common/TokenCommon.h>
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

    /// <summary>객체가 토큰을 얻었을 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnTokenEnter(CharacterBase* source, int tokenID) = 0;

    /// <summary>객체가 토큰이 제거되었을 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnTokenExit(CharacterBase* source, int tokenID) = 0;

    /// <summary>객체의 QTE가 시작할 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnQTEStart(CharacterBase* source) = 0;

    /// <summary>객체의 QTE가 끝날 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnQTEEnd(CharacterBase* source) = 0;

    /*
    Pre:    장비 데이터를 바꿀 수 있음.
    */

    /*
    Post:   장비 데이터 영향을 안받음(하지만 무기 정보 확인해야할 수도 있으니까 무기 정보는 넘겨줌.),
            최종 파라미터 수정 가능
    */

    virtual void OnPrePlayerAttackCalculateChain(PlayerAttackData& attackerData, EnemyHitData& targetData)              = 0;
    virtual void OnPreEnemyAttackCalculateChain(EnemyAttackData& attackerData, PlayerHitData& targetData)               = 0;
    virtual void OnPrePlayerHitCalculateChain(EnemyAttackData& attackerData, PlayerHitData& targetData)                 = 0;
    virtual void OnPreEnemyHitCalculateChain(PlayerAttackData& attackerData, EnemyHitData& targetData)                  = 0;

    virtual void OnPostPlayerAttackCalculateChain(PlayerAttackData& attackerData, EnemyHitData& targetData, int& chain) = 0;
    virtual void OnPostEnemyAttackCalculateChain(EnemyAttackData& attackerData, PlayerHitData& targetData, int& chain)  = 0;
    virtual void OnPostPlayerHitCalculateChain(EnemyAttackData& attackerData, PlayerHitData& targetData, int& chain)    = 0;
    virtual void OnPostEnemyHitCalculateChain(PlayerAttackData& attackerData, EnemyHitData& targetData, int& chain)     = 0;
                 
    virtual void OnPrePlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData)             = 0;
    virtual void OnPreEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData)              = 0;
    virtual void OnPrePlayerHitCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData)                = 0;
    virtual void OnPreEnemyHitCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData)                 = 0;

    virtual void OnPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData, int& damage) = 0;
    virtual void OnPostEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData, int& damage) = 0;
    virtual void OnPostPlayerHitCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData, int& damage) = 0;
    virtual void OnPostEnemyHitCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData, int& damage)  = 0;

    virtual void OnRollRandomSpeed(CharacterBase* source, int& speed) = 0;

public:
    virtual bool                CanAdd(CharacterBase* owner) const      = 0;
    virtual bool                CanRemove(CharacterBase* owner) const   = 0;

    virtual int                 GetTokenOrder() const                   = 0;
    virtual int                 GetTokenID() const                      = 0;
    virtual int                 GetMaxStackCount() const                = 0;
    virtual const std::string&  GetTokenName() const                    = 0;
    virtual const std::string&  GetTokenTag() const                     = 0;
    virtual int                 GetTokenParam(size_t index) const       = 0;
};