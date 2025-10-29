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
    ReflectFields->Actions.clear();
    if (false == _actions.empty())
    {
        for (auto& action : _actions)
        {
            if (action)
            {
                const std::string& name = action->ActionName;
                std::string        data = action->SerializedReflectFields();
                ReflectFields->Actions.emplace_back(name, data);
            }          
        }
    }
}

void AccessoryElement::DeserializedReflectEvent() 
{
    _actions.clear();
    if (false == ReflectFields->Actions.empty())
    {
        const auto& actionFactory = TurnActionFactory::GetActionFactory();
        for (auto& [name, data] : ReflectFields->Actions)
        {       
            if (auto iter = actionFactory.find(name); iter != actionFactory.end())
            {
                auto& myAction = _actions.emplace_back();
                myAction.reset(iter->second());
                myAction->DeserializedReflectFields(data);
            }
        }       
    }   
}


void AccessoryElement::DeepCopyAction(const std::vector<std::unique_ptr<TurnAction>>& actions)
{
    _actions.clear();
    if (false == actions.empty())
    {
        const auto& actionFactory = TurnActionFactory::GetActionFactory();
        for (auto& action : actions)
        {
            if (action)
            {
                const std::string& actionName = action->ActionName;          
                if (auto iter = actionFactory.find(actionName); iter != actionFactory.end())
                {
                    auto& myAction = _actions.emplace_back();
                    myAction.reset(iter->second());
                    if (myAction)
                    {
                        *myAction = *action;
                    }                 
                }
            }
        }
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
