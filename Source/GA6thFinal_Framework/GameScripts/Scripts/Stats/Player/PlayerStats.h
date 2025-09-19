#pragma once
#include "Stats/CharacterStats.h"
#include "ViewModels/Hp/CharacterHPViewModel.h"

struct PlayerStats : public CharacterStats
{
    inline static constexpr const char* MODEL_HP_KEY = "A5576328-0510-4E9C-A161-0868109710A9";

    USING_PROPERTY(PlayerStats)
    PlayerStats() = default;
    ~PlayerStats() override = default;

    REFLECT_PROPERTY(Shield)

    SETTER(int, Shield) { ReflectFields->Shield = std::clamp(value, 1, 999); }
    GETTER(int, Shield) { return ReflectFields->Shield; }
    PROPERTY(Shield)

protected:
    REFLECT_FIELDS_BEGIN(CharacterStats)
    int Shield = 0;
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

public:
    void RegisterHP()
    {
        std::string key = MODEL_HP_KEY;
        UmWatcher.Unregister<CharacterHPViewModel>(key);
        UmWatcher.Register<CharacterHPViewModel>(key, _hpModel);
    }

    void UnregisterHP()
    {
        std::string key = MODEL_HP_KEY;
        UmWatcher.Unregister<CharacterHPViewModel>(key);
    }
};