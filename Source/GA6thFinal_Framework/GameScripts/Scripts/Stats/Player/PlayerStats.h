#pragma once
#include "Stats/CharacterStats.h"
#include "ViewModels/Hp/CharacterHPViewModel.h"
#include "ViewModels/Chain/ChainCountViewModel.h"
#include "ViewModels/SturnResistance/SturnResistanceViewModel.h"

struct PlayerStats : public CharacterStats
{
    inline static constexpr const char* MODEL_HP_KEY        = "A5576328-0510-4E9C-A161-0868109710A9";
    inline static constexpr const char* MODEL_CHAIN_KEY     = "775124A6-1CD5-4222-912A-30BC74876430";
    inline static constexpr const char* MODEL_STURN_KEY     = "7EF594ED-1A69-48A5-8A7A-47D15F6A4E86";

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
    void RegisterHUD()
    {
        std::string hpkey = MODEL_HP_KEY;
        UmWatcher.Unregister<CharacterHPViewModel>(hpkey);
        UmWatcher.Register<CharacterHPViewModel>(hpkey, _hpModel);

        std::string chainkey = MODEL_CHAIN_KEY;
        UmWatcher.Unregister<ChainCountViewModel>(chainkey);
        UmWatcher.Register<ChainCountViewModel>(chainkey, _currentChainCount);

        std::string sturnKey = MODEL_STURN_KEY;
        UmWatcher.Unregister<SturnResistanceViewModel>(sturnKey);
        UmWatcher.Register<SturnResistanceViewModel>(sturnKey, _sturnResistanceModel);
    }

    void UnregisterHUD()
    {
        std::string hpKey = MODEL_HP_KEY;
        UmWatcher.Unregister<CharacterHPViewModel>(hpKey);

        std::string chainkey = MODEL_CHAIN_KEY;
        UmWatcher.Unregister<ChainCountViewModel>(chainkey);

        std::string sturnKey = MODEL_STURN_KEY;
        UmWatcher.Unregister<SturnResistanceViewModel>(sturnKey);
    }
};