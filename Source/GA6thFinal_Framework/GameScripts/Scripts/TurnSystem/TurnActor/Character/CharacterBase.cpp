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
    _mp(0), 
    _chainCount(0) , 
    _chainRoundCount(1) 
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
    _mp = MaxMP;
}

void CharacterBase::OnRoundStart() 
{
    Base::OnRoundStart();
    DecrementChainRoundCount();
}

void CharacterBase::Dead() 
{
    Base::Dead();
    _hp = 0;
    _mp = 0;
}

