#pragma once
#include "Stats/CharacterStats.h"
#include "ViewModels/Hp/CharacterHPViewModel.h"
#include "ViewModels/Chain/ChainCountViewModel.h"

struct EnemyStats : public CharacterStats
{
    EnemyStats() = default;
    ~EnemyStats() override = default;

    USING_PROPERTY(EnemyStats)
    REFLECT_PROPERTY(Speed)

    SETTER(int, Speed) { ReflectFields->Speed = std::clamp(value, -99, 99); }
    GETTER(int, Speed) { return ReflectFields->Speed; }
    PROPERTY(Speed)

    SETTER(int, Damage) { ReflectFields->Damage = value; }
    GETTER(int, Damage) { return ReflectFields->Damage; }
    PROPERTY(Damage)

    SETTER(float, DamageMultiplier) { ReflectFields->DamageMultiplier = value; }
    GETTER(float, DamageMultiplier) { return ReflectFields->DamageMultiplier; }
    PROPERTY(DamageMultiplier)

protected:
    REFLECT_FIELDS_BEGIN(CharacterStats)
    int Speed = 0;
    int Damage = 0;
    float DamageMultiplier = 1.0f;
    REFLECT_FIELDS_END(EnemyStats)
        
public:
    EnemyStats& CopyStats(const EnemyStats& rhs)
    {
        if (this != &rhs)
        {
            //reflect_fields_struct& myFields  = *this->ReflectFields;
            //reflect_fields_struct& rhsFields = *this->ReflectFields;
            //myFields                         = rhsFields;
            ReflectFields->Speed = rhs.ReflectFields->Speed;
            ReflectFields->Damage = rhs.ReflectFields->Damage;
            ReflectFields->DamageMultiplier = rhs.ReflectFields->DamageMultiplier;
        }
        return *this;
    }
    EnemyStats(const EnemyStats& rhs) { CopyStats(rhs); }
    EnemyStats& operator=(const EnemyStats& rhs) { return CopyStats(rhs); }

public:
    void RegisterHUD(const std::string& key)
    {
        if (false == key.empty())
        {
            UmWatcher.Unregister<CharacterHPViewModel>(key);
            UmWatcher.Register<CharacterHPViewModel>(key, _hpModel);

            UmWatcher.Unregister<ChainCountViewModel>(key);
            UmWatcher.Register<ChainCountViewModel>(key, _currentChainCount);
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_ERROR, "RegisterHUD key is empty.");
        }
    }
};
