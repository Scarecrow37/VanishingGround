#pragma once
#include "TurnSystem/TurnAction/Actions/TokenApplyAction/Base/TokenApplyAction.h"
#include "WeaponSystem/WeaponElement/WeaponElement.h"

//라운드 시작 시 특정 무기 종류 개수만큼 토큰 획득
class RoundStartWeaponTypeTokenApplyAction : public TokenApplyAction
{
    USING_PROPERTY(RoundStartWeaponTypeTokenApplyAction)
public:
    inline static const std::string NAME = (const char*)u8"라운드 시작 시 특정 무기 종류 개수만큼 토큰 획득";

    RoundStartWeaponTypeTokenApplyAction() = default;
    ~RoundStartWeaponTypeTokenApplyAction() override = default;

public:
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    const std::string& GetActionName() override;
    void               OnRoundStart() override;

protected:
    REFLECT_PROPERTY(ReflectFields->Type)

    REFLECT_FIELDS_BEGIN(TokenApplyAction)
    WeaponType Type = WeaponType::SWORD;
    REFLECT_FIELDS_END(RoundStartWeaponTypeTokenApplyAction)
};
