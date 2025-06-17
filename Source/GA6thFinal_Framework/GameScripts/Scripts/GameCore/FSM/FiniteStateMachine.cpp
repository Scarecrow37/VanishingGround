#include "FiniteStateMachine.h"
using namespace u8_literals;

constexpr int ORDER_MIN = std::numeric_limits<int>::min();

FiniteStateMachine::FiniteStateMachine() 
    : 
    _currState(nullptr), 
    _nextState(nullptr),
    _nextOrder(ORDER_MIN)
{

}

FiniteStateMachine::~FiniteStateMachine() = default;

bool FiniteStateMachine::IsValidEntryPoint()
{
    int startIndex = ReflectFields->EntryTransitionID;
    if (0 <= startIndex && startIndex < _transitions.size())
    {
        return true;
    }
    else
    {
        ReflectFields->EntryTransitionID = -1;
        return false;
    }
}

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

    ReflectFields->TransitionReflectDatas.clear();
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

    IsValidEntryPoint();
}

void FiniteStateMachine::ImguiDrawTransition() 
{
    int removeIndex = -1;
    if (ImGui::BeginTable("Transition", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("Entry");
        ImGui::TableSetupColumn("State");
        ImGui::TableSetupColumn("Condition");
        ImGui::TableSetupColumn("Next");
        ImGui::TableSetupColumn("Remove");
        ImGui::TableHeadersRow();

        for (int i = 0; i < _transitions.size(); ++i)
        {
            Transition& transition = _transitions[i];
            ImGui::PushID(&transition);
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if (i != ReflectFields->EntryTransitionID)
                {
                    if (ImGui::Button("Set entry"))
                    {
                        ReflectFields->EntryTransitionID = i;
                    }
                }
                else
                {
                    ImGui::Text("Entry");
                }
                ImGui::TableSetColumnIndex(1);
                ImGui::Text(typeid(*transition.CurrState).name() + 6);
                ImGui::TableSetColumnIndex(2);
                ImGui::Text(typeid(*transition.Condition).name() + 6);
                ImGui::TableSetColumnIndex(3);
                ImGui::Text(typeid(*transition.NextState).name() + 6);
                ImGui::TableSetColumnIndex(4);
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
        EraseTransition(removeIndex);
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

void FiniteStateMachine::Awake() 
{
    OnAwakeFSMEntities();

    if (true == IsValidEntryPoint())
    {
        Transition& transition = _transitions[ReflectFields->EntryTransitionID];
        _currState = transition.CurrState;
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"FSM의 EntryPoint가 설정되지 않았습니다."_c_str);
    }
}

void FiniteStateMachine::Start() 
{
    OnStartFSMEntities();

    if (nullptr != _currState)
    {
        _currState->OnEnter();
    }
}

void FiniteStateMachine::EraseTransition(int index) 
{
    _transitionSet.erase(_transitions[index]);
    _transitions.erase(_transitions.begin() + index);
}

void FiniteStateMachine::CheckTransitionCodition(Transition& transition) 
{
    if (true == transition.Condition->Evaluate())
    {
        _nextState = transition.NextState;
        _nextOrder = transition.Condition->ReflectFields->Order;
    }
}

void FiniteStateMachine::OnAwakeFSMEntities()
{
    for (auto& [key, state] : _stateMap)
    {
        state->OnAwake();
    }
    for (auto& [key, condition] : _conditionMap)
    {
        condition->OnAwake();
    }
}

void FiniteStateMachine::OnStartFSMEntities() 
{
    for (auto& [key, state] : _stateMap)
    {
        state->OnStart();
    }
    for (auto& [key, condition] : _conditionMap)
    {
        condition->OnStart();
    }
}

void FiniteStateMachine::ChangeTransition()
{
    if (nullptr != _currState)
    {
        for (auto& transition : _transitions)
        {
            if (transition.CurrState == _currState)
            {
                int conditionOrder = transition.Condition->ReflectFields->Order;
                if (nullptr != _nextState && conditionOrder < _nextOrder)
                {
                    CheckTransitionCodition(transition);
                }
                else
                {
                    CheckTransitionCodition(transition);
                }
            }
        }
    }

    if (nullptr != _nextState)
    {
        _currState->OnExit();
        _currState = _nextState;
        _currState->OnEnter();
        _nextState = nullptr;
        _nextOrder = ORDER_MIN;
    }
}

void FiniteStateMachine::UpdateTransition() 
{
    if (nullptr != _currState)
    {
        _currState->OnUpdate();
    }
}

void FiniteStateMachine::Update() 
{
    ChangeTransition();
    UpdateTransition();
}
