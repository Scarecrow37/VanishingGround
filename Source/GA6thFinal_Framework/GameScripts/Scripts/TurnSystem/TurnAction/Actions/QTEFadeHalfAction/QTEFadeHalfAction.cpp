#include "pchScripts.h"
#include "QTEFadeHalfAction.h"
#include "QTE/System/QTESystem.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"

REFLECT_FUNCTION(QTEFadeHalfAction)

REGISTER_TURN_ACTION(QTEFadeHalfAction)

const std::string& QTEFadeHalfAction::GetActionName()
{
    return NAME;
}

const std::string& QTEFadeHalfAction::GetActionInfo()
{
    return NAME;
}

void QTEFadeHalfAction::ImGuiDrawActionEditor()
{
    ImGuiDrawPropertys();
    ImguiDrawConditionEditor();
}

void QTEFadeHalfAction::OnEquipAccessory()
{
    if (ReflectFields->Trigger == TriggerType::EQUIP_ACCESSORY)
    {
        if (QTESystem* system = SingletonComponent<QTESystem>::GetInstance())
        {
            if (EvaluateConditions())
            {
                QTE::FadeState state;
                state.FadeInStartXFactor = 0.4f;
                state.FadeInEndXFactor   = 0.5f;

                system->SetFadeState(state);
            }
        }      
    }
}
