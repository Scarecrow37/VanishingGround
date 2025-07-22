#include "pchScripts.h"
#include "TurnAction.h"

using namespace u8_literals;

bool TurnAction::EvaluateConditions()
{  
    bool functionResult = EvaluateConditionFunctions();
    if (false == functionResult)
    {
        return false;
    }

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

TurnAction::FunctionHandle TurnAction::AddConditionFunction(const std::function<bool()>& func)
{
    FunctionHandle id = _nextId++;
    _evaluateListeners.emplace_back(id, func);
    _handleToIndexMap[id] = _evaluateListeners.size() - 1;
    return id;
}

bool TurnAction::RemoveConditionFunction(FunctionHandle id)
{
    // 1. Find the vector index corresponding to the handle from the map (O(log N))
    auto it = _handleToIndexMap.find(id);
    if (it == _handleToIndexMap.end())
    {
        return false;
    }
    size_t indexOfListenerToRemove = it->second;

    // 2. Swap the element to be removed with the last element in the vector
    //    (This is an O(1) operation for vectors)
    size_t lastIndex = _evaluateListeners.size() - 1;
    if (indexOfListenerToRemove != lastIndex)
    {
        // Swap with the last element
        std::swap(_evaluateListeners[indexOfListenerToRemove], _evaluateListeners[lastIndex]);

        // Update the index mapping for the element that was moved
        FunctionHandle handleOfMovedListener     = _evaluateListeners[indexOfListenerToRemove].first;
        _handleToIndexMap[handleOfMovedListener] = indexOfListenerToRemove;
    }

    // 3. Remove the last element from the vector (now the one we wanted to delete)
    _evaluateListeners.pop_back();

    // 4. Remove the original handle from the map
    _handleToIndexMap.erase(it);

    return true;
}

void TurnAction::ClearConditionFunction() 
{
    _evaluateListeners.clear();
    _handleToIndexMap.clear();
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

bool TurnAction::EvaluateConditionFunctions()
{
    bool result = _condtionFunctionOperator == ConditionOperator::AND ? true : false;
    switch (_condtionFunctionOperator)
    {
    case TurnAction::ConditionOperator::AND:
        for (auto& [id, func] : _evaluateListeners)
        {
            result &= func();
            if (result == false)
            {
                return false;
            }
        }
        return result;
    case TurnAction::ConditionOperator::OR:
        for (auto& [id, func] : _evaluateListeners)
        {
            result |= func();
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

