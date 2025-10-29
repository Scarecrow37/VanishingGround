#include "pchScripts.h"
#include "WeaponElement.h"
#include <TurnSystem/TurnAction/TurnActionFactory.h>

REFLECT_FUNCTION(WeaponElement)

void WeaponElement::SerializedReflectEvent() 
{
    ReflectFields->WeaponStatsData = Stats.SerializedReflectFields();
    ReflectFields->Actions.clear();
    if (false == _actions.empty())
    {
        for (auto& action : _actions)
        {
            if (action)
            {
                std::string name = action->ActionName;
                std::string data = action->SerializedReflectFields();
                ReflectFields->Actions.emplace_back(name, data);
            }       
        }   
    }
}

void WeaponElement::DeserializedReflectEvent() 
{
    Stats.DeserializedReflectFields(ReflectFields->WeaponStatsData);
    if (false == ReflectFields->Actions.empty())
    {
        _actions.clear();
        const auto& factory = TurnActionFactory::GetActionFactory();
        for (auto& [actionName, data] : ReflectFields->Actions)
        {          
            if (auto findIter = factory.find(actionName); findIter != factory.end())
            {
                TurnAction* newAction = findIter->second();
                if (newAction)
                {
                    auto& myAction = _actions.emplace_back();
                    myAction.reset(newAction);
                    myAction->DeserializedReflectFields(data);
                }                         
            }
        }     
    }
}

void WeaponElement::ImGuiDrawPropertysEvent() 
{
    Stats.ImGuiDrawPropertys();
    const std::string& name = Stats.WeaponName;
    for (auto& action : _actions)
    {
        if (action)
        {
            const std::string& name = action->ActionName;
            if(ImGui::TreeNode(name.c_str()))
            {
                static std::string info;
                info = action->ActionInfo;
                ImGui::InputText("Action##CAEE12AA-CE07-4816-951D-242031D9186B", &info, ImGuiInputTextFlags_ReadOnly);
                ImGui::TreePop();
            }       
        }
    }
}

void WeaponElement::DeepCopyAction(const std::vector<std::unique_ptr<TurnAction>>& rhs)
{
    _actions.clear();
    const auto& actionFactory = TurnActionFactory::GetActionFactory();
    for (auto& action : rhs)
    {
        if (action)
        {
            const std::string& actionName = action->ActionName;
            if (auto iter = actionFactory.find(actionName); iter != actionFactory.end())
            {
                auto& myAction = _actions.emplace_back();
                myAction.reset(iter->second());
                *myAction = *action;
            }
        }     
    }
}

DropItemInfo WeaponElement::GetItemInfo() const
{
    DropItemInfo info
    {
        .Category = GetCategoryType(),
        .ID = Stats.WeaponID,
        .Name = (const std::string&)Stats.WeaponName,
    };
    return info;
}

ArtifactDropType WeaponElement::GetCategoryType() const
{
    WeaponType type = Stats.Type;
    switch (type)
    {
    case WeaponType::SWORD:
        return ArtifactDropType::SWORD;
    case WeaponType::DAGGER:
        return ArtifactDropType::DAGGER;
    case WeaponType::WARHAMMER:
        return ArtifactDropType::WARHAMMER;
    default:
        return ArtifactDropType::SWORD;
    }
}
