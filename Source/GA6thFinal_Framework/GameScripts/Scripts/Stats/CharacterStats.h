#pragma once
#include "TurnActorStats.h"

struct CharacterStats : public TurnActorStats
{
    USING_PROPERTY(CharacterStats)
    CharacterStats() = default;
    ~CharacterStats() override = default;

    REFLECT_PROPERTY(MaxHP, MaxMP, MaxChainRoundCount)

    SETTER(int, MaxHP) { ReflectFields->MaxHP = std::clamp(value, 1, 99999); }
    GETTER(int, MaxHP) { return ReflectFields->MaxHP; }
    PROPERTY(MaxHP)

    SETTER(int, MaxMP) { ReflectFields->MaxMP = std::clamp(value, 1, 999); }
    GETTER(int, MaxMP) { return ReflectFields->MaxMP; }
    PROPERTY(MaxMP)
      
    SETTER(int, MaxChainRoundCount) { ReflectFields->MaxChainRoundCount = std::max(value, 1); }
    GETTER(int, MaxChainRoundCount) { return ReflectFields->MaxChainRoundCount; }
    //연격 수가 유지되는 라운드 수. 기본값 : 1
    PROPERTY(MaxChainRoundCount)

protected:
    REFLECT_FIELDS_BEGIN(TurnActorStats)
    int MaxHP      = 100;
    int MaxMP      = 100;
    int MaxChainRoundCount = 1;
    REFLECT_FIELDS_END(CharacterStats)

};

