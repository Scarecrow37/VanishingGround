#pragma once
#include "TurnSystem/TurnAction/TurnAction.h"

class EquipAccessoryCharacterStats : public TurnAction
{
    USING_PROPERTY(EquipAccessoryCharacterStats)
public:
    inline static const std::string ACTION_NAME = (const char*)u8"악세서리로 장착시 캐릭터의 스텟 증가";

protected:
    REFLECT_PROPERTY(
        ReflectFields->Target,
        ReflectFields->MaxHP,
        ReflectFields->CurrentHP,
        ReflectFields->MaxChainRoundCount,
        ReflectFields->StunResistance
    )

    REFLECT_FIELDS_BEGIN(TurnAction)
    TurnTarget Target             = TurnTarget::PLAYER;
    int        MaxHP              = 0;
    int        CurrentHP          = 0;
    int        MaxChainRoundCount = 0;
    int        StunResistance     = 0;
    REFLECT_FIELDS_END(EquipAccessoryCharacterStats)

    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;

    void OnEquipAccessory() override;
};