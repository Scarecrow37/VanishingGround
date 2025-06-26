#pragma once
#include "../TurnActor.h"

struct CharacterStats;
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

    GETTER_ONLY(int, MaxHP) { return GetMaxHP(); }
    PROPERTY(MaxHP)

    GETTER_ONLY(int, MaxMP) { return GetMaxMP(); }
    PROPERTY(MaxMP)
    
    GETTER_ONLY(int, ChainCount) { return GetChainCount(); }
    PROPERTY(ChainCount)

    GETTER_ONLY(int, HP) { return _hp; }
    PROPERTY(HP)

    GETTER_ONLY(int, MP) { return _mp; }
    PROPERTY(MP)

private:
    int GetMaxHP();
    int GetMaxMP();
    int GetChainCount();

public:
    CharacterBase();
    virtual ~CharacterBase();

protected:
    virtual CharacterStats* GetCharacterStats() = 0;

protected:
    REFLECT_FIELDS_BEGIN(TurnActor)
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

    virtual void OnRevive() override;
    virtual void OnDead() override;
};
