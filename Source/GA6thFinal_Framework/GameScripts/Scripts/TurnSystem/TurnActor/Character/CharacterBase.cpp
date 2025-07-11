#include "pchScripts.h"
#include "CharacterBase.h"
#include "Stats/CharacterStats.h"

int CharacterBase::GetMaxHP()
{
    int maxHP = 0;
    CharacterStats* stats = GetCharacterStats();
    if (nullptr != stats)
    {
        maxHP = stats->MaxHP;
    }
    return maxHP;
}

int CharacterBase::GetMaxMP()
{
    int maxMP = 0;
    CharacterStats* stats = GetCharacterStats();
    if (nullptr != stats)
    {
        maxMP = stats->MaxMP;
    }
    return maxMP;
}

int CharacterBase::GetMaxChainRoundCount()
{
    int maxChainCount = 1;
    CharacterStats* stats = GetCharacterStats();
    if (nullptr != stats)
    {
        maxChainCount = stats->MaxChainRoundCount;
    }
    return maxChainCount;
}

CharacterBase::CharacterBase() : 
    _hp(0), 
    _chainCount(0) , 
    _chainRoundCount(1) ,
    _tokenManager(this)
{
}

CharacterBase::~CharacterBase() = default;

void CharacterBase::Awake() 
{
    Base::Awake();
    gameObject->AddTag(TAG);
}

void CharacterBase::Revive() 
{
    Base::Revive();
    _hp = MaxHP;
}

void CharacterBase::Dead()
{
    Base::Dead();
    _hp = 0;

    _tokenManager.NotifyDead();
}

void CharacterBase::OnCombatStart() 
{
    _tokenManager.NotifyCombatStart();
}

void CharacterBase::OnRoundStart()
{
    Base::OnRoundStart();
    DecrementChainRoundCount();
    _tokenManager.NotifyRoundStart();
}

void CharacterBase::OnRoundEnd()
{
    Base::OnRoundEnd();
    _tokenManager.NotifyRoundEnd();
}

void CharacterBase::OnTurnStart()
{
    Base::OnTurnStart();
    _tokenManager.NotifyTurnStart();
}

void CharacterBase::OnTurnEnd() 
{
    Base::OnTurnEnd();
    _tokenManager.NotifyTurnEnd();
}

void CharacterBase::OnHit() 
{
    Base::OnHit();
    _tokenManager.NotifyHit();
}

void CharacterBase::OnDead() 
{
    Base::OnDead();
    _tokenManager.NotifyDead();
}

void CharacterBase::OnKill(CharacterBase* destination) 
{
    Base::OnKill(destination);
    _tokenManager.NotifyKill(destination);
}

void CharacterBase::OnTokenAdded(int tokenID) 
{
    Base::OnTokenAdded(tokenID);
    _tokenManager.NotifyTokenAdded(tokenID);
}

void CharacterBase::OnTokenRemoved(int tokenID) 
{
    Base::OnTokenRemoved(tokenID);
    _tokenManager.NotifyTokenRemoved(tokenID);
}
