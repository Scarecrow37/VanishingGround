#pragma once
#include "UmFramework.h"
struct CharacterStats : public ReflectSerializer
{
    USING_PROPERTY(CharacterStats)
    REFLECT_PROPERTY(MaxHP, MaxMP, ChainCount)

    SETTER(int, MaxHP) { ReflectFields->MaxHP = std::clamp(value, 1, 99999); }
    GETTER(int, MaxHP) { return ReflectFields->MaxHP; }
    PROPERTY(MaxHP)

    SETTER(int, MaxMP) { ReflectFields->MaxMP = std::clamp(value, 1, 999); }
    GETTER(int, MaxMP) { return ReflectFields->MaxMP; }
    PROPERTY(MaxMP)

    SETTER(int, ChainCount) { ReflectFields->ChainCount = std::clamp(value, 0, 99); }
    GETTER(int, ChainCount) { return ReflectFields->ChainCount; }
    PROPERTY(ChainCount)

protected:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    int MaxHP      = 0;
    int MaxMP      = 0;
    int ChainCount = 0;
    REFLECT_FIELDS_END(CharacterStats)

};

