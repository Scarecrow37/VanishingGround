#include "pchScripts.h"
#include "RevelationElement.h"
#include <RevelationSystem/RevelationSystem.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

bool RevelationElement::Evaluate(CharacterBase* attacker, CharacterBase* target)
{
    bool result = false;
    int chainCount = target->ChainCount;
    RevelationConditionType condition  = ReflectFields->Condition;
    switch (condition)
    {
    case RevelationConditionType::GREATER_THAN_OR_EQUAL:
        result = chainCount >= ReflectFields->ConditionValueA;
        break;
    case RevelationConditionType::LESS_THAN_OR_EQUAL:
        result = chainCount <= ReflectFields->ConditionValueA;
        break;
    case RevelationConditionType::BETWEEN_INCLUSIVE:
        result = ReflectFields->ConditionValueA <= chainCount && chainCount <= ReflectFields->ConditionValueA;
        break;
    case RevelationConditionType::EQUAL:
        result = chainCount == ReflectFields->ConditionValueA;
        break;
    case RevelationConditionType::MULTIPLE_OF:
        result = chainCount % ReflectFields->ConditionValueA == 0;
        break;
    default:
        break;
    }
    return result;
}

void RevelationElement::ImGuiDrawPropertysEvent()
{
    RevelationSystem* system = RevelationSystem::GetInstance();
    if (system)
    {
        std::string_view selectName = STR_NULL;
        if (_action)
        {
            selectName = _action->GetActionInfo();
        }

        if (_action)
        {
            ImGui::Checkbox("Edit", &_showActionEditor);
            ImGui::SameLine();
        }

        if (ImGui::BeginCombo("Action", selectName.data()))
        {
            for (auto& [key, func] : system->GetActionFactory())
            {
                if (ImGui::Selectable(key.data()))
                {
                    _action.reset(func());
                }
            }
            ImGui::EndCombo();
        }

        if (_action)
        {
            if (_showActionEditor)
                _action->ImGuiDrawActionEditor();
        }
    }
    _imguiDrawIndex = 0;
}

void RevelationElement::SerializedReflectEvent() 
{
    if (_action)
    {
        ReflectFields->ActionName = (std::string_view)_action->Name;
    }    
    else
    {
        ReflectFields->ActionName = STR_NULL;
    }
}

void RevelationElement::DeserializedReflectEvent() 
{
    RevelationSystem* system        = RevelationSystem::GetInstance();
    const auto&       actionFactory = system->GetActionFactory();
    auto              iter          = actionFactory.find(ReflectFields->ActionName.data());

    if (system)
    {
        if (iter != actionFactory.end())
        {
            _action.reset(iter->second());
        }
    }
}

void RevelationElement::DeepCopyAction(const RevelationActionBase& action) 
{
    RevelationSystem* system        = RevelationSystem::GetInstance();
    const auto&       actionFactory = system->GetActionFactory();
    std::string_view  actionName    = action.Name;
    auto              iter          = actionFactory.find(actionName.data());

    if (system)
    {
        if (iter != actionFactory.end())
        {
            _action.reset(iter->second());
            RevelationActionBase& rhs  = const_cast<RevelationActionBase&>(action);
            std::string           data = rhs.SerializedReflectFields();
            _action->DeserializedReflectFields(data);
        }
    }
}
