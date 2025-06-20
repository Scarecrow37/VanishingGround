#pragma once
#include "../TurnActor.h"

class CharacterBase abstract : public TurnActor
{
    USING_PROPERTY(CharacterBase)
public:
    inline static constexpr const char* TAG = "Character";

public:
    REFLECT_PROPERTY(
        HP,
        MP,
        MaxHP, 
        MaxMP, 
        ChainCount
        )

    SETTER(int, MaxHP)
    { 
        ReflectFields->MaxHP = std::clamp(value, 1, 99999);       
    }
    GETTER(int, MaxHP)
    {
        return ReflectFields->MaxHP;
    }
    PROPERTY(MaxHP)

    SETTER(int, MaxMP) { ReflectFields->MaxMP = std::clamp(value, 1, 999); }
    GETTER(int, MaxMP) { return ReflectFields->MaxMP; }
    PROPERTY(MaxMP)
    
    SETTER(int, ChainCount) { ReflectFields->ChainCount = std::clamp(value, 0, 99); }
    GETTER(int, ChainCount) { return ReflectFields->ChainCount; }
    PROPERTY(ChainCount)

    int SetHP(int value) { return _hp = std::clamp(value, 0, ReflectFields->MaxHP); }
    GETTER_ONLY(int, HP) { return _hp; }
    PROPERTY(HP)

    int SetMP(int value) { return _mp = std::clamp(value, 0, ReflectFields->MaxMP); }
    GETTER_ONLY(int, MP) { return _mp; }
    PROPERTY(MP)

public:
    CharacterBase();
    virtual ~CharacterBase();

protected:
    REFLECT_FIELDS_BEGIN(TurnActor)
    int MaxHP = 0;
    int MaxMP = 0;
    int ChainCount = 0;
    REFLECT_FIELDS_END(CharacterBase)

private:
    int _hp;
    int _mp;

protected:
    /// <summary>
    /// <para> 이 함수는 항상 Start 함수 전에 호출되며 프리팹이 인스턴스화 된 직후에 호출됩니다.                </para>
    /// <para> 게임 오브젝트의 Active가 false 상태인 경우 Awake 함수는 true가 될때까지 호출되지 않습니다.      </para>
    /// </summary>
    virtual void Awake();

    void OnRevive() override;
};
