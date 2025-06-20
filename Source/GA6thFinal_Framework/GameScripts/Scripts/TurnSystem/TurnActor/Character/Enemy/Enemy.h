#pragma once
#include "../CharacterBase.h"

class Enemy : public CharacterBase
{
    USING_PROPERTY(Enemy)
public:
    inline static constexpr const char* TAG = "Enemy";

public:
    REFLECT_PROPERTY(
        Speed
        )

    SETTER(int, Speed) { ReflectFields->Speed = std::clamp(value, -99, 99); }
    GETTER(int, Speed) { return ReflectFields->Speed; }
    PROPERTY(Speed)
public:
    Enemy();
    virtual ~Enemy();

protected:
    REFLECT_FIELDS_BEGIN(CharacterBase)
    int Speed = 0;
    REFLECT_FIELDS_END(Enemy)

public:
    int GetSpeed() override;

protected:
    /// <summary>
    /// <para> 이 함수는 항상 Start 함수 전에 호출되며 프리팹이 인스턴스화 된 직후에 호출됩니다.                </para>
    /// <para> 게임 오브젝트의 Active가 false 상태인 경우 Awake 함수는 true가 될때까지 호출되지 않습니다.      </para>
    /// </summary>
    virtual void Awake();

    virtual void Update();

    // CharacterBase을(를) 통해 상속됨
    void OnTurnStart() override;
    void OnTurnEnd() override;
};
