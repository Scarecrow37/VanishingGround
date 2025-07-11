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
    _tokenSystem(this)
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

    _tokenSystem.NotifyDead();
}

void CharacterBase::OnCombatStart() 
{
    _tokenSystem.NotifyCombatStart();
}

void CharacterBase::OnRoundStart()
{
    Base::OnRoundStart();
    DecrementChainRoundCount();
    _tokenSystem.NotifyRoundStart();
}

void CharacterBase::OnRoundEnd()
{
    Base::OnRoundEnd();
    _tokenSystem.NotifyRoundEnd();
}

void CharacterBase::OnTurnStart()
{
    Base::OnTurnStart();
    _tokenSystem.NotifyTurnStart();
}

void CharacterBase::OnTurnEnd() 
{
    Base::OnTurnEnd();
    _tokenSystem.NotifyTurnEnd();
}

void CharacterBase::OnHit() 
{
    Base::OnHit();
    _tokenSystem.NotifyHit();
}

void CharacterBase::OnDead() 
{
    Base::OnDead();
    _tokenSystem.NotifyDead();
}

void CharacterBase::OnKill(CharacterBase* destination) 
{
    Base::OnKill(destination);
    _tokenSystem.NotifyKill(destination);
}

void CharacterBase::OnTokenAdded(int tokenID) 
{
    Base::OnTokenAdded(tokenID);
    _tokenSystem.NotifyTokenAdded(tokenID);
}

void CharacterBase::OnTokenRemoved(int tokenID) 
{
    Base::OnTokenRemoved(tokenID);
    _tokenSystem.NotifyTokenRemoved(tokenID);
}
