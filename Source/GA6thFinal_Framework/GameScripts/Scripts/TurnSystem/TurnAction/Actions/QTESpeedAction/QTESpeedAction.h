#pragma once
#include "TurnSystem/TurnAction/TurnAction.h"

class QTESpeedAction : public TurnAction
{
    USING_PROPERTY(QTESpeedAction)
    inline static const std::string NAME = (const char*)u8"QTE 속도를 조절합니다.";
public:
    enum class TriggerType
    {
        EQUIP_ACCESSORY,
    };

    QTESpeedAction() = default;
    ~QTESpeedAction() = default;
   
protected:
    REFLECT_PROPERTY(
        ReflectFields->Trigger,
        ReflectFields->Speed
    )

    REFLECT_FIELDS_BEGIN(TurnAction)
    TriggerType Trigger = TriggerType::EQUIP_ACCESSORY;
    int         Speed   = 0;
    REFLECT_FIELDS_END(QTESpeedAction)
    
// TurnAction을(를) 통해 상속됨
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;

    void OnEquipAccessory() override;
};

