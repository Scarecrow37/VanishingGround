#include "FiniteStateMachine.h"
using namespace u8_literals;

FiniteStateMachine::FiniteStateMachine()  = default;
FiniteStateMachine::~FiniteStateMachine() = default;

void FiniteStateMachine::SerializedReflectEvent() 
{
    for (auto& [key, state] : _stateMap)
    {
        ReflectFields->StateReflectDatas[key] = state->SerializedReflectFields();
    }

    for (auto& [key, condition] : _conditionMap)
    {
        ReflectFields->ConditionReflectDatas[key] = condition->SerializedReflectFields();
    }

    for (auto& transition : _transitions)
    {
        const char* currState = typeid(*transition.CurrState).name();
        const char* nextState = typeid(*transition.NextState).name();
        const char* condition = typeid(*transition.Condition).name();
        ReflectFields->TransitionReflectDatas.push_back({currState, condition, nextState});
    }
}

void FiniteStateMachine::DeserializedReflectEvent() 
{
    for (auto& [key, data] : ReflectFields->StateReflectDatas)
    {
        FSMState* state = FSMStateFactory::NewInstanceWithKey(key);
        if (state)
        {
            _stateMap[key].reset(state);
            _stateMap[key]->DeserializedReflectFields(data);
        }
    }

    for (auto& [key, data] : ReflectFields->ConditionReflectDatas)
    {
        FSMCondition* condition = FSMConditionFactory::NewInstanceWithKey(key);
        if (condition)
        {
            _conditionMap[key].reset(condition);
            _conditionMap[key]->DeserializedReflectFields(data);
        }
    }

    for (auto& transition : ReflectFields->TransitionReflectDatas)
    {
        std::string_view currState = transition[0];
        std::string_view condition = transition[1];
        std::string_view nextState = transition[2];
        AddTransition(currState, condition, nextState);
    }
}

void FiniteStateMachine::ImguiDrawTransition() 
{
    int removeIndex = -1;
    if (ImGui::BeginTable("Transition", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("State");
        ImGui::TableSetupColumn("Condition");
        ImGui::TableSetupColumn("Next");
        ImGui::TableHeadersRow();

        for (int i = 0; i < _transitions.size(); ++i)
        {
            Transition& transition = _transitions[i];
            ImGui::PushID(&transition);
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text(typeid(*transition.CurrState).name() + 6);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text(typeid(*transition.Condition).name() + 6);
                ImGui::TableSetColumnIndex(2);
                ImGui::Text(typeid(*transition.NextState).name() + 6);
                ImGui::TableSetColumnIndex(3);
                if (ImGui::Button("Remove"))
                {
                    removeIndex = i;
                }
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    if (0 <= removeIndex)
    {
        _transitions.erase(_transitions.begin() + removeIndex);
    }

    if (ImGui::Button("Add Transition"))
    {
        ImGui::OpenPopup("Add Transition");
    }

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 center = ImVec2(viewport->Pos.x + viewport->Size.x * 0.5f, viewport->Pos.y + viewport->Size.y * 0.5f);
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopup("Add Transition", ImGuiWindowFlags_NoMove))
    {
        static std::string state;
        static std::string condition;
        static std::string nextState;
        ImGui::BeginDisabled();
        ImGui::InputText("State", &state);
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::BeginMenu("##State"))
        {
            const char* stateSel = AddStateImguiPopUp();
            if (stateSel)
            {
                state = stateSel;
            }
            ImGui::EndMenu();
        }
        ImGui::BeginDisabled();
        ImGui::InputText("Condition", &condition);
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::BeginMenu("##Condition"))
        {
            const char* conditionSel = AddConditionImguiPopup();
            if (conditionSel)
            {
                condition = conditionSel;
            }
            ImGui::EndMenu();
        }
        ImGui::BeginDisabled();
        ImGui::InputText("Next state", &nextState);
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::BeginMenu("##Next state"))
        {
            const char* stateSel = AddStateImguiPopUp();
            if (stateSel)
            {
                nextState = stateSel;
            }
            ImGui::EndMenu();
        }
        if (false == state.empty()     && 
            false == condition.empty() && 
            false == nextState.empty())
        {
            if (state != nextState)
            {
                if (ImGui::Button("Add"))
                {
                    AddTransition(state, condition, nextState);
                    state.clear();
                    condition.clear();
                    nextState.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
            }
        }
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

const char* FiniteStateMachine::AddStateImguiPopUp()
{
    const char* addKey = nullptr;
    if (ImGui::BeginChild("ConstructorsChild", ImVec2(0, 100), ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_AlwaysVerticalScrollbar))
    {
        static ImGuiTextFilter filter;
        filter.Draw("Search");
        for (auto& [key, func] : FSMStateFactory::GetInstanceConstructors())
        {
            if (filter.PassFilter(key.c_str() + 6))
            {
                if (ImGui::Selectable(key.c_str() + 6))
                {
                    addKey = key.c_str();
                }
            }
        }
        ImGui::EndChild();
    }
    return addKey;
}

const char* FiniteStateMachine::AddConditionImguiPopup()
{
    const char* addKey = nullptr;
    if (ImGui::BeginChild("ConstructorsChild", ImVec2(0, 100), ImGuiChildFlags_AutoResizeX,
        ImGuiWindowFlags_AlwaysVerticalScrollbar))
    {
        static ImGuiTextFilter filter;
        filter.Draw("Search");
        for (auto& [key, func] : FSMConditionFactory::GetInstanceConstructors())
        {
            if (filter.PassFilter(key.c_str() + 6))
            {
                if (ImGui::Selectable(key.c_str() + 6))
                {
                    addKey = key.c_str();
                }
            }
        }
        ImGui::EndChild();
    }
    return addKey;
}

void FiniteStateMachine::ImguiDrawCondiitons() 
{
    const char* removeKey = nullptr;
    for (auto& [key, condition] : _conditionMap)
    {
        ImGui::PushID(&condition);
        {
            ImGui::Text(key.c_str() + 6);
            ImGui::SameLine();
            if (ImGui::Button("Remove"))
            {
                removeKey = key.c_str();
            }
        }
        ImGui::PopID();
    }

    if (nullptr != removeKey)
    {
        RemoveConditionWithKey(removeKey);
    }
}

void FiniteStateMachine::AddTransition(std::string_view state, std::string_view condition, std::string_view nextState)
{
    bool isValid = true;
    auto currStateIter = _stateMap.find(state.data());
    auto conditionIter = _conditionMap.find(condition.data());
    auto nextStateIter = _stateMap.find(nextState.data());
    isValid &= currStateIter != _stateMap.end();
    isValid &= conditionIter != _conditionMap.end();
    isValid &= nextStateIter != _stateMap.end();
    if (true == isValid)
    {
        Transition trans{};
        trans.CurrState = currStateIter->second.get();
        trans.Condition = conditionIter->second.get();
        trans.NextState = nextStateIter->second.get();
        auto [iter, result] = _transitionSet.insert(trans);
        if (true == result)
        {
            _transitions.push_back(trans);
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"이미 추가된 전이 객체 입니다."_c_str);
        }
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"찾을수 없는 전이 객체 입니다."_c_str);
    }
}

void FiniteStateMachine::ImguiDrawStates() 
{
    const char* removeKey = nullptr;
    for (auto& [key, state] : _stateMap)
    {
        ImGui::PushID(&state);
        {
            ImGui::Text(key.c_str() + 6);
            ImGui::SameLine();
            if (ImGui::Button("Remove"))
            {
                removeKey = key.c_str();
            }
        }
        ImGui::PopID();        
    }

    if (nullptr != removeKey)
    {
        RemoveStateWithKey(removeKey);
    }
}

void FiniteStateMachine::ImGuiDrawPropertysEvent() 
{
    ImGui::PushID(this);
    {
        ImguiDrawTransition();

        if (ImGui::CollapsingHeader("States"))
        {
            ImguiDrawStates();
            if (true == ImGui::Button("Add State"))
            {
                ImGui::OpenPopup("AddStatePopup");
            }
        }
        if (ImGui::BeginPopup("AddStatePopup"))
        {
            const char* addKey = AddStateImguiPopUp();
            if (nullptr != addKey)
            {
                AddStateWithKey(addKey);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (ImGui::CollapsingHeader("Conditions"))
        {
            ImguiDrawCondiitons();
            if (true == ImGui::Button("Add Condition"))
            {
                ImGui::OpenPopup("AddConditionPopup");
            }
        }
        if (ImGui::BeginPopup("AddConditionPopup"))
        {
            const char* addKey = AddConditionImguiPopup();
            if (nullptr != addKey)
            {
                AddConditionWithKey(addKey);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    ImGui::PopID();
}
