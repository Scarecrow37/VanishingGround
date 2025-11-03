#pragma once
#include "TurnSystem/TurnAction/TurnAction.h"

class QTEFadeHalfAction : public TurnAction
{
    USING_PROPERTY(QTEFadeHalfAction)
    inline static const std::string NAME = (const char*)u8"QTE 화면을 절반으로 조절합니다.";

public:
    enum class TriggerType
    {
        EQUIP_ACCESSORY,
    };

    QTEFadeHalfAction() = default;
    ~QTEFadeHalfAction() = default;

protected:
    REFLECT_PROPERTY(ReflectFields->Trigger)

    REFLECT_FIELDS_BEGIN(TurnAction)
    TriggerType Trigger = TriggerType::EQUIP_ACCESSORY;
    REFLECT_FIELDS_END(QTEFadeHalfAction)

    // TurnAction을(를) 통해 상속됨
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;

    void OnEquipAccessory() override;
};
