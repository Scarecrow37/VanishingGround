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

int CharacterBase::GetChainCount()
{
    int chainCount =0;
    CharacterStats* stats = GetCharacterStats();
    if (nullptr != stats)
    {
        chainCount = stats->ChainCount;
    }
    return chainCount;
}

CharacterBase::CharacterBase() : 
    _hp(0), 
    _mp(0)
{

}
CharacterBase::~CharacterBase() = default;

void CharacterBase::Awake() 
{
    Base::Awake();
    gameObject->AddTag(TAG);
}

void CharacterBase::OnRevive() 
{
    _hp = MaxHP;
    _mp = MaxMP;
}

void CharacterBase::OnDead() 
{
    _hp = 0;
    _mp = 0;
}

