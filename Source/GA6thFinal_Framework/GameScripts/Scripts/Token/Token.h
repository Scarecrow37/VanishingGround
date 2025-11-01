#pragma once
#include "Interface/IToken.h"

// @brief 토큰의 기본 멤버입니다. 이걸 추가하지 않으면 System에 추가가 불가능합니다
#define TOKEN_DATA(id)                                              \
public:                                                             \
static constexpr int ID = id;                                       \
inline int GetTokenID() const  override { return ID; }              

class Token : public IToken
{
    USING_PROPERTY(Token)
    friend class TokenSystem;

public:
    Token();
    ~Token() override;

public:
    // 콜백에 대한 자세한 주석은 ITriggerType.h를 참고하세요.
    virtual void OnCombatStart(CharacterBase* source) override                                  {}
    virtual void OnRoundStart(CharacterBase* owner) override                                    {}
    virtual void OnRoundEnd(CharacterBase* owner) override                                      {}
    virtual void OnEachTurnStart(CharacterBase* source, CharacterBase* destination) override    {}
    virtual void OnTurnStart(CharacterBase* owner) override                                     {}
    virtual void OnTurnEnd(CharacterBase* owner) override                                       {}
    virtual void OnHit(CharacterBase* owner) override                                           {}
    virtual void OnDead(CharacterBase* owner) override                                          {}
    virtual void OnKill(CharacterBase* source, CharacterBase* destination) override             {}
    virtual void OnTokenAdded(CharacterBase* owner, int tokenID) override                       {}
    virtual void OnTokenRemoved(CharacterBase* owner, int tokenID) override                     {}
    virtual void OnTokenEnter(CharacterBase* owner, int tokenID) override                       {}
    virtual void OnTokenExit(CharacterBase* owner, int tokenID) override                        {}
    virtual void OnQTEStart(CharacterBase* owner) override                                      {}
    virtual void OnQTEEnd(CharacterBase* owner) override                                        {}

    virtual void OnPrePlayerAttackCalculateChain(PlayerAttackData& attackerData, EnemyHitData& targetData)                  override {}
    virtual void OnPreEnemyAttackCalculateChain(EnemyAttackData& attackerData, PlayerHitData& targetData)                   override {}
    virtual void OnPrePlayerHitCalculateChain(EnemyAttackData& attackerData, PlayerHitData& targetData)                     override {}
    virtual void OnPreEnemyHitCalculateChain(PlayerAttackData& attackerData, EnemyHitData& targetData)                      override {}

    virtual void OnPostPlayerAttackCalculateChain(PlayerAttackData& attackerData, EnemyHitData& targetData, int& chain)     override {}
    virtual void OnPostEnemyAttackCalculateChain(EnemyAttackData& attackerData, PlayerHitData& targetData, int& chain)      override {}
    virtual void OnPostPlayerHitCalculateChain(EnemyAttackData& attackerData, PlayerHitData& targetData, int& chain)        override {}
    virtual void OnPostEnemyHitCalculateChain(PlayerAttackData& attackerData, EnemyHitData& targetData, int& chain)         override {}
    
    virtual void OnPrePlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData)                 override {}                                                      
    virtual void OnPreEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData)                  override {}                                                      
    virtual void OnPrePlayerHitCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData)                    override {}
    virtual void OnPreEnemyHitCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData)                     override {}

    virtual void OnPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData, int& damage)   override {}                                                      
    virtual void OnPostEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData, int& damage)    override {}                                                      
    virtual void OnPostPlayerHitCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData, int& damage)      override {}
    virtual void OnPostEnemyHitCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData, int& damage)       override {}

    virtual void OnRollRandomSpeed(CharacterBase* source, int& speed) override {}

public:
    virtual bool CanAdd(CharacterBase* owner) const override;
    virtual bool CanRemove(CharacterBase* owner) const override;

    inline const std::string&   GetTokenName()      const override { return _tokenData.Name; }
    inline const std::string&   GetTokenTag()       const override { return _tokenData.Tag; }
    inline int                  GetTokenOrder()     const override { return _tokenData.Order; }
    inline int                  GetMaxStackCount()  const override { return _tokenData.MaxStack; }
    int                         GetTokenParam(size_t index) const override;

protected:
    std::string TokenLog(CharacterBase& dest);

protected:
    TokenData _tokenData;

    std::function<void(int)> _dirtyOrderCallback = nullptr; // 우선순위가 변경되었을 때 호출되는 콜백 함수
};