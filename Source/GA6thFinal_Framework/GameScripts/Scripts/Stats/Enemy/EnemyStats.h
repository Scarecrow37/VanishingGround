#pragma once
#include "Stats/CharacterStats.h"

struct EnemyStats : public CharacterStats
{
    EnemyStats() = default;
    ~EnemyStats() override = default;

    USING_PROPERTY(EnemyStats)
    REFLECT_PROPERTY(Speed)

    SETTER(int, Speed) { ReflectFields->Speed = std::clamp(value, -99, 99); }
    GETTER(int, Speed) { return ReflectFields->Speed; }
    PROPERTY(Speed)

protected:
    REFLECT_FIELDS_BEGIN(CharacterStats)
    int Speed = 0;
    REFLECT_FIELDS_END(EnemyStats)
        
public:
    EnemyStats& CopyStats(const EnemyStats& rhs)
    {
        if (this != &rhs)
        {
            reflect_fields_struct& myFields  = *this->ReflectFields;
            reflect_fields_struct& rhsFields = *this->ReflectFields;
            myFields                         = rhsFields;
        }
        return *this;
    }
    EnemyStats(const EnemyStats& rhs) { CopyStats(rhs); }
    EnemyStats& operator=(const EnemyStats& rhs) { return CopyStats(rhs); }

};