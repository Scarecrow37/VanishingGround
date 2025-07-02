#pragma once
#include "Stats/CharacterStats.h"

struct PlayerStats : public CharacterStats
{
    USING_PROPERTY(PlayerStats)
    REFLECT_PROPERTY(ManaRegenRate, Shield)

    SETTER(int, ManaRegenRate) { ReflectFields->ManaRegenRate = std::clamp(value, 1, 999); }
    GETTER(int, ManaRegenRate) { return ReflectFields->ManaRegenRate; }
    PROPERTY(ManaRegenRate)

    SETTER(int, Shield) { ReflectFields->Shield = std::clamp(value, 1, 999); }
    GETTER(int, Shield) { return ReflectFields->Shield; }
    PROPERTY(Shield)

protected:
    REFLECT_FIELDS_BEGIN(CharacterStats)
    int ManaRegenRate = 0;
    int Shield        = 0;
    REFLECT_FIELDS_END(PlayerStats)

};