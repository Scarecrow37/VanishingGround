#pragma once
#include "Stats/CharacterStats.h"

struct PlayerStats : public CharacterStats
{
    USING_PROPERTY(PlayerStats)
    REFLECT_PROPERTY(Shield)

    SETTER(int, Shield) { ReflectFields->Shield = std::clamp(value, 1, 999); }
    GETTER(int, Shield) { return ReflectFields->Shield; }
    PROPERTY(Shield)

protected:
    REFLECT_FIELDS_BEGIN(CharacterStats)
    int Shield        = 0;
    REFLECT_FIELDS_END(PlayerStats)

public:
    PlayerStats& CopyStats(const PlayerStats& rhs)
    {
        if (this != &rhs)
        {
            reflect_fields_struct& myFields  = *this->ReflectFields;
            reflect_fields_struct& rhsFields = *this->ReflectFields;
            myFields                         = rhsFields;
        }
        return *this;
    }
    PlayerStats(const PlayerStats& rhs) { CopyStats(rhs); };
    PlayerStats& operator=(const PlayerStats& rhs) { return CopyStats(rhs); };

};