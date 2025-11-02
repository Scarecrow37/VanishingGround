#include "pchScripts.h"
#include "QTESpeedAction.h"

#include "TurnSystem/TurnAction/TurnActionFactory.h"

REFLECT_FUNCTION(QTESpeedAction)

REGISTER_TURN_ACTION(QTESpeedAction)

const std::string& QTESpeedAction::GetActionName()
{
    return NAME;
}

const std::string& QTESpeedAction::GetActionInfo()
{
    return NAME;
}

void QTESpeedAction::ImGuiDrawActionEditor() 
{
    ImGuiDrawPropertys();
    ImguiDrawConditionEditor();
}

void QTESpeedAction::OnEquipAccessory() 
{
    if (ReflectFields->Trigger == TriggerType::EQUIP_ACCESSORY)
    {
        if (EvaluateConditions())
        {
            //TODO: QTE 속도 제어해야함
            int speed = ReflectFields->Speed;
        }
    }
}
