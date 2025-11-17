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
            *ReflectFields          = *rhs.ReflectFields;
            _hpModel                = rhs._hpModel.Get();
            _currentHP              = rhs._currentHP;
            _reduceHpModel          = rhs._reduceHpModel.Get();
            _currentChainCount      = rhs._currentChainCount.Get();
            _currentChainRoundCount = rhs._currentChainRoundCount;
            _sturnResistanceModel   = rhs._sturnResistanceModel.Get();
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

            std::string newKey = key + "_reduce";
            UmWatcher.Unregister<CharacterHPViewModel>(newKey);
            UmWatcher.Register<CharacterHPViewModel>(newKey, _reduceHpModel);

            UmWatcher.Unregister<ChainCountViewModel>(key);
            UmWatcher.Register<ChainCountViewModel>(key, _currentChainCount);
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_ERROR, "RegisterHUD key is empty.");
        }
    }
};
