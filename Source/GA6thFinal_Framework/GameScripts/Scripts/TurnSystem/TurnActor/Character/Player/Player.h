#pragma once
#include "../CharacterBase.h"
class Player : public CharacterBase
{
    USING_PROPERTY(Player)
public:
    inline static constexpr const char* TAG = "Player";

public:
    REFLECT_PROPERTY(
        ManaRegenRate, 
        Shield
    )

    SETTER(int, ManaRegenRate)
    {
        ReflectFields->ManaRegenRate = std::clamp(value,1, 999);
    }
    GETTER(int, ManaRegenRate) 
    { 
        return ReflectFields->ManaRegenRate;
    }
    PROPERTY(ManaRegenRate)

    SETTER(int, Shield) { ReflectFields->Shield = std::clamp(value, 1, 999); }
    GETTER(int, Shield) { return ReflectFields->Shield; }
    PROPERTY(Shield)

public:
    Player();
    virtual ~Player();

protected:
    REFLECT_FIELDS_BEGIN(CharacterBase)
    int ManaRegenRate = 0;
    int Shield        = 0;
    REFLECT_FIELDS_END(Player)

public:
    // CharacterBase을(를) 통해 상속됨
    int GetSpeed() override;

protected:
    /// <summary>
    /// <para> 이 함수는 항상 Start 함수 전에 호출되며 프리팹이 인스턴스화 된 직후에 호출됩니다.                </para>
    /// <para> 게임 오브젝트의 Active가 false 상태인 경우 Awake 함수는 true가 될때까지 호출되지 않습니다.      </para>
    /// </summary>
    virtual void Awake();
    
    /// <summary>
    /// Update 는 프레임당 한 번 호출됩니다.
    /// </summary>
    virtual void Update();

    // CharacterBase을(를) 통해 상속됨
    void OnTurnStart() override;
    void OnTurnEnd() override;
};
