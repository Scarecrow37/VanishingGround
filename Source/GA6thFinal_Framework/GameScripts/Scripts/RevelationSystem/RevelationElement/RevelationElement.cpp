#include "pchScripts.h"
#include "RevelationElement.h"
#include <RevelationSystem/RevelationSystem.h>

#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnAction/TurnActionFactory.h>
#include <ItemDropSystem/UI/ItemDropUIRootManager.h>

using namespace u8_literals;

REFLECT_FUNCTION(RevelationElement)

void RevelationElement::ImGuiDrawPropertysEvent()
{
    
}

void RevelationElement::SerializedReflectEvent() 
{
    if (_action)
    {
        ReflectFields->ActionName = _action->ActionName;
    }    
    else
    {
        ReflectFields->ActionName = STR_NULL;
    }
}

void RevelationElement::DeserializedReflectEvent() 
{
    const auto&       actionFactory = TurnActionFactory::GetActionFactory();
    auto              iter          = actionFactory.find(ReflectFields->ActionName);
    if (iter != actionFactory.end())
    {
        _action.reset(iter->second());
    }
}

void RevelationElement::DeepCopyAction(const TurnAction& action)
{
    const auto&        actionFactory = TurnActionFactory::GetActionFactory();
    const std::string& actionName    = action.ActionName;
    auto               iter          = actionFactory.find(actionName);
    if (iter != actionFactory.end())
    {
        _action.reset(iter->second());
        *_action = action;
    }  
}

DropItemInfo RevelationElement::GetItemInfo() const
{
    DropItemInfo info
    {
        .Category = ArtifactDropType::REVELATION,
        .ID = RevelationID, 
        .Name = (const std::string&)ElementName,
    };
    return info;
}

