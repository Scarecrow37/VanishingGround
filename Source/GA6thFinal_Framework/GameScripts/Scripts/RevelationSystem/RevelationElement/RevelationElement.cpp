#include "pchScripts.h"
#include "RevelationElement.h"
#include <RevelationSystem/RevelationSystem.h>

#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnAction/TurnActionFactory.h>

using namespace u8_literals;

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

        if (ImGui::BeginCombo("##Action", selectName.data()))
        {
            for (auto& [key, func] : TurnActionFactory::GetActionFactory())
            {
                if (ImGui::Selectable(key.data()))
                {
                    _action.reset(func());
                }
            }
            ImGui::EndCombo();
        }
        else
        {
            static std::string toolTip;
            toolTip.clear();
            if (_action)
            {
                toolTip = _action->GetConditionsInfo();
            }
            toolTip += selectName;
            ImGuiHelper::HoveredToolTip(toolTip);
        }

        if (_action)
        {
            if (_showActionEditor)
            {
                static std::string id;
                id.clear();
                id = "Action Editor###";
                id += ReflectFields->Name;
                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                ImGui::Begin(id.c_str(), &_showActionEditor);
                {
                    _action->ImGuiDrawActionEditor();
                }
                ImGui::End();
            }        

            ImGui::SameLine();
            if (ImGui::Button("Action Editer"))
            {
                _showActionEditor = !_showActionEditor;
            }
        }
    }
    _imguiDrawIndex = 0;
}

void RevelationElement::SerializedReflectEvent() 
{
    if (_action)
    {
        ReflectFields->ActionName = _action->Name;
    }    
    else
    {
        ReflectFields->ActionName = STR_NULL;
    }
}

void RevelationElement::DeserializedReflectEvent() 
{
    RevelationSystem* system        = RevelationSystem::GetInstance();
    const auto&       actionFactory = TurnActionFactory::GetActionFactory();
    auto              iter          = actionFactory.find(ReflectFields->ActionName.data());

    if (system)
    {
        if (iter != actionFactory.end())
        {
            _action.reset(iter->second());
        }
    }
}

void RevelationElement::DeepCopyAction(const TurnAction& action)
{
    RevelationSystem*  system        = RevelationSystem::GetInstance();
    const auto&        actionFactory = TurnActionFactory::GetActionFactory();
    const std::string& actionName    = action.Name;
    auto               iter          = actionFactory.find(actionName);

    if (system)
    {
        if (iter != actionFactory.end())
        {
            _action.reset(iter->second());
            TurnAction& rhs  = const_cast<TurnAction&>(action);
            std::string data = rhs.SerializedReflectFields();
            _action->DeserializedReflectFields(data);
        }
    }
}

const std::string& RevelationElement::GetActionName()
{
    const static std::string actionName = u8"계시"_c_str;
    return actionName;
}

const std::string& RevelationElement::GetActionInfo()
{
    const static std::string actionInfo = u8"계시"_c_str;
    return actionInfo;
}

void RevelationElement::ImGuiDrawActionEditor() 
{

}

void RevelationElement::OnPlayerBattleStart(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats,
                                            Enemy& target, EnemyStats& targetStats)
{
    if (IsAction())
    {
        _action->OnPlayerBattleStart(attacker, attackerStats, weaponStats, target, targetStats);
    }
}
