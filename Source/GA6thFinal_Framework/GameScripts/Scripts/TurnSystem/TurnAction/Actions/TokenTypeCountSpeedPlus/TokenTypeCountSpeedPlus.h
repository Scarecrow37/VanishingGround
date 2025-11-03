#pragma once
#pragma once
#include "TurnSystem/TurnAction/TurnActionFactory.h"

class Player;
struct PlayerStats;
struct WeaponStats;
class Enemy;
struct EnemyStats;

// 스피드 계산할때 토큰 태그 가진 캐릭터들 만큼 추가 속도
class TokenTypeCountSpeedPlus : public TurnAction
{
    USING_PROPERTY(TokenTypeCountSpeedPlus)
public:
    TokenTypeCountSpeedPlus();
    ~TokenTypeCountSpeedPlus() override;

protected:
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;

    GETTER(const std::string&, TokenTag) { return ReflectFields->TokenTag; }
    SETTER(const std::string&, TokenTag)
    {
        ReflectFields->TokenTag = value;
        UpdateActionInfo();
    }
    PROPERTY(TokenTag)

    GETTER(int, PlusSpeed) { return ReflectFields->PlusSpeed; }
    SETTER(int, PlusSpeed)
    {
        ReflectFields->PlusSpeed = value;
        UpdateActionInfo();
    }
    PROPERTY(PlusSpeed)

    REFLECT_FIELDS_BEGIN(TurnAction)
    TurnTarget  TokenTarget    = TurnTarget::ENEMY;
    std::string TokenTag       = (const char*)u8"출혈";
    int         PlusSpeed      = 1;
    REFLECT_FIELDS_END(TokenTypeCountSpeedPlus)

private:
    void OnWeaponRoundSpeedApply(WeaponElement& weapon, int& plusSpeed) override;

    void TryTokenSystemInfoUpdate();
    bool _validTokenSystem = false;

    void        UpdateActionInfo();
    std::string _actionInfo;
};