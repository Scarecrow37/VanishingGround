#include "pchScripts.h"
#include "TurnAction.h"
#include <RevelationSystem/RevelationSystem.h>
#include <TurnSystem/TurnAction/TurnActionFactory.h>

using namespace u8_literals;

namespace
{
    template<typename T>
    void ImGuiDrawActionMakerEx(std::string_view windowID, T& action, bool& showActionEditor)
    {
        std::string_view selectName = STR_NULL;
        if (action)
        {
            selectName = action->GetActionInfo();
        }

        if (ImGui::BeginCombo("##Action", selectName.data()))
        {
            for (auto& [key, func] : TurnActionFactory::GetActionFactory())
            {
                if (ImGui::Selectable(key.data()))
                {
                    action.reset(func());
                }
            }
            ImGui::EndCombo();
        }
        else
        {
            static std::string toolTip;
            toolTip.clear();
            if (action)
            {
                toolTip = action->GetConditionsInfo();
            }
            toolTip += selectName;
            ImGuiHelper::HoveredToolTip(toolTip);
        }

        if (action)
        {
            if (showActionEditor)
            {
                static std::string id;
                id.clear();
                id = "Action Editor###";
                id += windowID;
                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                ImGui::Begin(id.c_str(), &showActionEditor);
                {
                    action->ImGuiDrawActionEditor();
                }
                ImGui::End();
            }

            ImGui::SameLine();
            if (ImGui::Button("Action Editer"))
            {
                showActionEditor = !showActionEditor;
            }
        }    
    }
}

void TurnAction::ImGuiDrawActionMaker(std::string_view windowID, std::unique_ptr<TurnAction>& action,
                                      bool& showActionEditor)
{
    ImGuiDrawActionMakerEx(windowID, action, showActionEditor);
}

void TurnAction::ImGuiDrawActionMaker(std::string_view windowID, std::shared_ptr<TurnAction>& action,
                                      bool& showActionEditor)
{
    ImGuiDrawActionMakerEx(windowID, action, showActionEditor);
}

void TurnAction::ImGuiDrawActionMaker(std::string_view windowID, TurnAction& action, bool& showActionEditor) 
{
    static std::string actionName;
    actionName = action.ActionName;;
    ImGui::InputText(windowID.data(), &actionName, ImGuiInputTextFlags_ReadOnly);
    static std::string toolTip;
    toolTip.clear();
    toolTip = action.GetConditionsInfo();
    ImGuiHelper::HoveredToolTip(toolTip);

    if (showActionEditor)
    {
        static std::string id;
        id.clear();
        id = "Action Editor###";
        id += windowID;
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::Begin(id.c_str(), &showActionEditor);
        {
            action.ImGuiDrawActionEditor();
        }
        ImGui::End();
    }

    ImGui::SameLine();
    if (ImGui::Button("Action Editer"))
    {
        showActionEditor = !showActionEditor;
    }
}

bool TurnAction::EvaluateConditions()
{  
   bool result = EvaluateConditionsEx();
   for (auto& condition : _conditions)
   {
        condition->OnEvaluateConditions(result);
   }
   return result;
}

const std::string& TurnAction::GetConditionsInfo() const
{
    ConditionOperator logic = LogicOperator;
    static std::string info;
    info.clear();
    auto& conditions = GetConditions();
    int   lastIndex  = (int)conditions.size() - 1;
    for (int i = 0; i < conditions.size(); ++i)
    {
        auto& condition = conditions[i];
        info += condition->GetConditionInfo();
        if (i != lastIndex)
        {
            std::string_view oper;
            switch (logic)
            {
            case ConditionOperator::AND:
                oper = u8"이고"_c_str;
                break;
            case ConditionOperator::OR:
                oper = u8"이거나"_c_str;
                break;
            }
            info += oper;
        }
        else
        {
            info += u8"이면"_c_str;
        }
        info += "\n";
    }
    return info;
}


void TurnAction::SerializedReflectEvent()
{
    ConditionsToReflectDatas();
}

void TurnAction::DeserializedReflectEvent() 
{
    ReflectDatasToConditions();
}

void TurnAction::ImguiDrawConditionEditor() 
{
    ImGui::PushID(this);
    {
        TurnActionCondition* eraseTemp = nullptr;
        for (auto& condition : _conditions)
        {
            auto RightClick = [&]() {
                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::MenuItem((const char*)u8"삭제"))
                    {
                        eraseTemp = condition.get();
                    }
                    ImGui::EndPopup();
                }
            };

            static std::string id;
            id.clear();
            id = condition->GetConditionInfo();
            id += "###20327F79-EFF5-486D-A05A-2D27A6387683";
            ImGui::PushID(&condition);
            if (ImGui::TreeNode(id.c_str()))
            {
                RightClick();
                condition->DrawImguiEditor();
                ImGui::TreePop();
            }
            else
            {
                RightClick();
            }
            ImGui::PopID();
        }

        if (eraseTemp != nullptr)
        {
            std::erase_if(_conditions, [eraseTemp](auto& unique) { return unique.get() == eraseTemp; });
        }

        static std::string_view selectValue = STR_NULL;
        if (ImGui::BeginCombo("##963EABCA-C1CE-414C-8B4C-9E9D3FFBD398", selectValue.data()))
        {
            for (auto& [key, func] : GetInstanceConstructors())
            {
                if (ImGui::Selectable(key.c_str(), key == selectValue))
                {
                    selectValue = key;
                }
            }
            ImGui::EndCombo();
        }

        ImGui::SameLine();
        if (ImGui::Button((const char*)u8"조건 추가하기") && selectValue != STR_NULL)
        {
            TurnActionCondition* condition = NewInstanceWithKey(selectValue);
            if (condition)
            {
                _conditions.emplace_back(condition);
            }
        }
    }
    ImGui::PopID();
}

void TurnAction::ConditionsToReflectDatas() 
{
    ReflectFields->_conditionDatas.clear();
    for (auto& condition : _conditions)
    {
        if (condition)
        {
            ReflectFields->_conditionDatas.emplace_back(typeid(*condition).name(),
                                                        condition->SerializedReflectFields());
        }     
    }
}

void TurnAction::ReflectDatasToConditions() 
{
    _conditions.clear();
    for (auto& [key, data] : ReflectFields->_conditionDatas)
    {
        TurnActionCondition* condition = NewInstanceWithKey(key);
        if (condition)
        {
            condition->DeserializedReflectFields(data);
            _conditions.emplace_back(condition);
        }     
    }
}

bool TurnAction::EvaluateConditionsEx()
{
    if (true == _conditions.empty())
    {
        return true;
    }

    ConditionOperator oper   = LogicOperator;
    bool              result = oper == ConditionOperator::AND ? true : false;
    switch (oper)
    {
    case ConditionOperator::AND:
        for (auto& condition : _conditions)
        {
            result &= condition->Evaluate();
            if (result == false)
            {
                return false;
            }
        }
        return result;
    case ConditionOperator::OR:
        for (auto& condition : _conditions)
        {
            result |= condition->Evaluate();
            if (result == true)
            {
                return true;
            }
        }
        return result;
    default:
        return false;
    }
}

