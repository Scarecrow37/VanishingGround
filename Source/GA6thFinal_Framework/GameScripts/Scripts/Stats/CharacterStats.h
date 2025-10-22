#pragma once
#include "TurnActorStats.h"

struct CharacterStats : public TurnActorStats
{
    USING_PROPERTY(CharacterStats)
    CharacterStats() = default;
    ~CharacterStats() override = default;

    REFLECT_PROPERTY(
        MaxHP, 
        MaxChainRoundCount,
        StunResistance,
        StunResistanceMultiplier
    )

    SETTER(int, MaxHP) 
    { 
        int maxHP = std::clamp(value, 1, 99999); 
        ReflectFields->MaxHP = maxHP;
        if (maxHP < _currentHP)
        {
            _currentHP = maxHP;
        }
        _hpModel = std::make_pair(_currentHP, maxHP);
    }
    GETTER(int, MaxHP) { return ReflectFields->MaxHP; }
    // int 최대 체력
    PROPERTY(MaxHP)

    SETTER(int, CurrentHP)
    {
        _currentHP = std::clamp(value, 0, ReflectFields->MaxHP);
        _hpModel   = std::make_pair(_currentHP, ReflectFields->MaxHP);
    }
    GETTER(int, CurrentHP) { return _currentHP; }
    // int 현재 체력
    PROPERTY(CurrentHP)

    SETTER(int, CurrentChainCount) { _currentChainCount = std::clamp(value, 0, 99); }
    GETTER(int, CurrentChainCount) { return _currentChainCount; }
    // int 현재 연격 수
    PROPERTY(CurrentChainCount)

    SETTER(int, CurrentChainRoundCount)
    {
        _currentChainRoundCount = std::clamp(value, 0, ReflectFields->MaxChainRoundCount);
    }
    GETTER(int, CurrentChainRoundCount) { return _currentChainRoundCount; }
    // int 연격 수 지속 시간
    PROPERTY(CurrentChainRoundCount)

    SETTER(int, MaxChainRoundCount) { ReflectFields->MaxChainRoundCount = std::max(value, 1); }
    GETTER(int, MaxChainRoundCount) { return ReflectFields->MaxChainRoundCount; }
    // int 연격 수가 유지되는 라운드 수. 기본값 : 1
    PROPERTY(MaxChainRoundCount)

    SETTER(int, StunResistance) 
    { 
        ReflectFields->StunResistance = std::max(value, 0); 
        _sturnResistanceModel = ReflectFields->StunResistance;
    }
    GETTER(int, StunResistance) { return ReflectFields->StunResistance; }
    // 기본 기절 저항 수치.
    PROPERTY(StunResistance)

    SETTER(float, StunResistanceMultiplier) { ReflectFields->StunResistanceMultiplier = std::max(value, 0.0f); }
    GETTER(float, StunResistanceMultiplier) { return ReflectFields->StunResistanceMultiplier; }
    PROPERTY(StunResistanceMultiplier)
    // 기절 저항 수치 획득 배율

protected:
    REFLECT_FIELDS_BEGIN(TurnActorStats)
    int MaxHP      = 100;
    int MaxMP      = 100;
    int MaxChainRoundCount = 1;
    int StunResistance     = 0;
    float StunResistanceMultiplier = 1.25f;
    REFLECT_FIELDS_END(CharacterStats)

    MVVM::Model<std::pair<int, int>> _hpModel;
    int _currentHP = 100;
    MVVM::Model<int> _currentChainCount = 0;
    int _currentChainRoundCount = 0;
    MVVM::Model<int> _sturnResistanceModel;

    void DeserializedReflectEvent() override;
};

