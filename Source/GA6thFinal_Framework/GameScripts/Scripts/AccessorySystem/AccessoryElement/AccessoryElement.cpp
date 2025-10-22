#include "pchScripts.h"
#include "AccessoryElement.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"

REFLECT_FUNCTION(AccessoryElement)

AccessoryElement::AccessoryElement() 
{

}

AccessoryElement::~AccessoryElement() 
{

}

void AccessoryElement::ImGuiDrawPropertysEvent() 
{

}

void AccessoryElement::SerializedReflectEvent() 
{
    if (_action)
    {
        ReflectFields->ActionName = _action->ActionName;
        ReflectFields->ActionData = _action->SerializedReflectFields();
    }
    else
    {
        ReflectFields->ActionName = STR_NULL;
        ReflectFields->ActionData = STR_NULL;
    }
}

void AccessoryElement::DeserializedReflectEvent() 
{
    const auto& actionFactory = TurnActionFactory::GetActionFactory();
    auto        iter          = actionFactory.find(ReflectFields->ActionName);
    if (iter != actionFactory.end())
    {
        _action.reset(iter->second());
        _action->DeserializedReflectFields(ReflectFields->ActionData);
    }
}


void AccessoryElement::DeepCopyAction(const TurnAction& action)
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

DropItemInfo AccessoryElement::GetItemInfo() const
{
    DropItemInfo info;
    info.Category = ArtifactDropType::ACCESSORY;
    info.ID       = ReflectFields->ID;
    info.Name     = ReflectFields->AccessoryName;
    return info;
}
