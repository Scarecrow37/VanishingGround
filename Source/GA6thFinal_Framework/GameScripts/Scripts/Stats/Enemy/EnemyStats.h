#pragma once
#include "Stats/CharacterStats.h"

struct EnemyStats : public CharacterStats
{
    USING_PROPERTY(EnemyStats)
    REFLECT_PROPERTY(Speed)

    SETTER(int, Speed) { ReflectFields->Speed = std::clamp(value, -99, 99); }
    GETTER(int, Speed) { return ReflectFields->Speed; }
    PROPERTY(Speed)

protected:
    REFLECT_FIELDS_BEGIN(CharacterStats)
    int Speed = 0;
    REFLECT_FIELDS_END(EnemyStats)

};