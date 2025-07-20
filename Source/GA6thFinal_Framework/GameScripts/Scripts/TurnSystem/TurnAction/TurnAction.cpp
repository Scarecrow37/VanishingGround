#include "pchScripts.h"
#include "TurnAction.h"

bool TurnAction::EvaluateConditions()
{
    SortConditions();
    bool result = true;
    for (auto& condition : _conditions)
    {
        TurnActionCondition::LogicalOperator oper = condition->LogicOperator;
        switch (oper)
        {
        case TurnActionCondition::LogicalOperator::AND:
            result &= condition->Evaluate();
            break;
        case TurnActionCondition::LogicalOperator::OR:
            result |= condition->Evaluate();
            break;
        default:
            result = false;
            break;
        }

        if (result == false)
        {
            break;
        }
    }
    return result;
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
    TurnActionCondition* eraseTemp = nullptr;
    for (auto& condition : _conditions)
    {
        auto RightClick = [&]() 
        {
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

    if (selectValue != STR_NULL)
    {
        ImGui::SameLine();
        if (ImGui::Button((const char*)u8"조건 추가하기"))
        {
            TurnActionCondition* condition = NewInstanceWithKey(selectValue);
            if (condition)
            {
                _conditions.emplace_back(condition);
            }
        }
    }
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

void TurnAction::SortConditions() 
{
    std::ranges::sort(_conditions, 
    [](std::unique_ptr<TurnActionCondition>& conditionA, std::unique_ptr<TurnActionCondition>& conditionB) 
    {
        int orderA = conditionA->Order;
        int orderB = conditionB->Order;
        return orderA < orderB;
    });
}
