#include "pchScripts.h"
#include "WeaponElement.h"
#include <TurnSystem/TurnAction/TurnActionFactory.h>

void WeaponElement::SerializedReflectEvent() 
{
    ReflectFields->WeaponStatsData = Stats.SerializedReflectFields();
    if (_action)
    {
        ReflectFields->ActionName = _action->ActionName;
        ReflectFields->ActionDatas = _action->SerializedReflectFields();
    }
    else
    {
        ReflectFields->ActionName  = STR_NULL;
        ReflectFields->ActionDatas = STR_NULL;
    }
}

void WeaponElement::DeserializedReflectEvent() 
{
    Stats.DeserializedReflectFields(ReflectFields->WeaponStatsData);
    if (ReflectFields->ActionName != STR_NULL)
    {
        const auto& factory = TurnActionFactory::GetActionFactory();
        auto findIter = factory.find(ReflectFields->ActionName);
        if (findIter != factory.end())
        {
            TurnAction* newAction = findIter->second();
            _action.reset(newAction);
            newAction->DeserializedReflectFields(ReflectFields->ActionDatas);
        }
    }
}

void WeaponElement::ImGuiDrawPropertysEvent() 
{
    Stats.ImGuiDrawPropertys();
    const std::string& name = Stats.WeaponName;
    if (_action)
    {
        static std::string name;
        name = _action->ActionName;
        ImGui::InputText("Action##CAEE12AA-CE07-4816-951D-242031D9186B", &name, ImGuiInputTextFlags_ReadOnly);
        ImGuiHelper::HoveredToolTip((const std::string&)_action->ActionInfo);
    }
}

void WeaponElement::DeepCopyAction(const TurnAction& rhs)
{
    const auto&        actionFactory = TurnActionFactory::GetActionFactory();
    const std::string& actionName    = rhs.ActionName;
    auto               iter          = actionFactory.find(actionName);
    if (iter != actionFactory.end())
    {
        _action.reset(iter->second());
        *_action = rhs;
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
