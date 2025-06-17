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
    if (0 <= startIndex && startIndex < _transitions.size() && nullptr != _transitions[startIndex].CurrState)
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
        const char* currState = transition.CurrState ? typeid(*transition.CurrState).name() : STR_NULL;
        const char* nextState = typeid(*transition.NextState).name();
        const char* condition = typeid(*transition.Condition).name();
        ReflectFields->TransitionReflectDatas.push_back({currState, condition, nextState});
    }
}

void FiniteStateMachine::DeserializedReflectEvent() 
{
    for (auto& [key, data] : ReflectFields->StateReflectDatas)
    {
        FSMState* state = MakeState(key);
        if (state)
        {
            state->_owner = this;
            _stateMap[key].reset(state);
            _stateMap[key]->DeserializedReflectFields(data);
        }
    }

    for (auto& [key, data] : ReflectFields->ConditionReflectDatas)
    {
        FSMCondition* condition = MakeCondition(key);
        if (condition)
        {
            condition->_owner = this;
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

void FiniteStateMachine::ImGuiDrawDebug() 
{
    //const char* entryStateName = IsValidEntryPoint() ? typeid(*_transitions[ReflectFields->EntryTransitionID].CurrState).name() : STR_NULL;
    //ImGui::Text("Entry : %s", entryStateName);
    const char* currStateName = _currState ? typeid(*_currState).name() : STR_NULL;
    ImGui::Text("State : %s", currStateName);
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
                bool isAnyState = nullptr == transition.CurrState;
                if (false == isAnyState)
                {
                    if (i != ReflectFields->EntryTransitionID)
                    {
                        if (ImGui::Button("Set entry"))
                        {
                            SetEntryTransition(i);
                        }
                    }
                    else
                    {
                        ImGui::Text("Entry");
                    }
                }
                ImGui::TableSetColumnIndex(1);
                const char* currState = transition.CurrState ? typeid(*transition.CurrState).name() + 6 : "Any State";
                ImGui::Text(currState);
                ImGui::TableSetColumnIndex(2);
                ImGui::Text(typeid(*transition.Condition).name() + 6);
                ImGui::DragInt("Order", &transition.Condition->ReflectFields->Order);
                ImGuiHelper::HoveredToolTip(u8"전이 조건의 우선 순위입니다. (낮을수록 우선됩니다.)"_c_str);
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
        {
            static std::string anyState = "Any State";
            std::string* pState = state == STR_NULL ? &anyState : &state;
            ImGui::InputText("State", pState);
        }     
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::BeginMenu("##State"))
        {
            const char* stateSel = SelectMyStateImguiChild(true);
            if (nullptr != stateSel)
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
            const char* conditionSel = SelectMyConditionImguiChild();
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
            const char* stateSel = SelectMyStateImguiChild();
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

const char* FiniteStateMachine::SelectStateImguiChild()
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

const char* FiniteStateMachine::SelectMyStateImguiChild(bool enableAnyState)
{
    const char* addKey = nullptr;
    if (ImGui::BeginChild("StateChild", ImVec2(0, 100), ImGuiChildFlags_AutoResizeX,
                          ImGuiWindowFlags_AlwaysVerticalScrollbar))
    {
        static ImGuiTextFilter filter;
        filter.Draw("Search");
        if (enableAnyState)
        {
            if (filter.PassFilter("Any State"))
            {
                if (ImGui::Selectable("Any State"))
                {
                    addKey = STR_NULL;
                }
                ImGuiHelper::HoveredToolTip(u8"어떤 상태에서든 전이가 가능한 상태입니다."_c_str);
            }
        }
        for (auto& [key, state] : _stateMap)
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

const char* FiniteStateMachine::SlectConditionImguiChild()
{
    const char* addKey = nullptr;
    if (ImGui::BeginChild("ConditionChild", ImVec2(0, 100), ImGuiChildFlags_AutoResizeX,
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

const char* FiniteStateMachine::SelectMyConditionImguiChild()
{
    const char* addKey = nullptr;
    if (ImGui::BeginChild("ConstructorsChild", ImVec2(0, 100), ImGuiChildFlags_AutoResizeX,
                          ImGuiWindowFlags_AlwaysVerticalScrollbar))
    {
        static ImGuiTextFilter filter;
        filter.Draw("Search");
        for (auto& [key, ptr] : _conditionMap)
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
    bool isAnyState = state == STR_NULL;
    auto currStateIter = _stateMap.find(state.data());
    auto conditionIter = _conditionMap.find(condition.data());
    auto nextStateIter = _stateMap.find(nextState.data());
    if (false == isAnyState)
    {
        isValid &= currStateIter != _stateMap.end();
    }   
    isValid &= conditionIter != _conditionMap.end();
    isValid &= nextStateIter != _stateMap.end();
    if (true == isValid)
    {
        Transition trans{};
        trans.CurrState = isAnyState ? nullptr : currStateIter->second.get();
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

void FiniteStateMachine::AddAnyTransition(std::string_view condition, std::string_view nextState) 
{
    AddTransition(STR_NULL, condition, nextState);
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
        ImGuiDrawDebug();

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
            const char* addKey = SelectStateImguiChild();
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
            const char* addKey = SlectConditionImguiChild();
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
        _nextState = transition.CurrState;
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"FSM의 EntryPoint가 설정되지 않았습니다."_c_str);
    }
}

void FiniteStateMachine::Start() 
{
    OnStartFSMEntities();
}

FSMState* FiniteStateMachine::MakeState(std::string_view key)
{
    FSMState* state = FSMStateFactory::NewInstanceWithKey(key);
    if (state)
    {
        state->_owner = this;
    }
    return state;
}

FSMCondition* FiniteStateMachine::MakeCondition(std::string_view key)
{
    FSMCondition* condition = FSMConditionFactory::NewInstanceWithKey(key);
    if (condition)
    {
        condition->_owner = this;
    }
    return condition;
}

void FiniteStateMachine::EraseTransition(int index)
{
    _transitionSet.erase(_transitions[index]);
    _transitions.erase(_transitions.begin() + index);
}

void FiniteStateMachine::SetEntryTransition(int index) 
{
    if (0 <= index && index < _transitions.size())
    {
        if (nullptr != _transitions[index].CurrState)
        {
            ReflectFields->EntryTransitionID = index;
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Any State는 Entry로 설정할 수 없습니다."_c_str);
        }     
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"존재하지 않는 전이 객체 입니다."_c_str);
    }
}

void FiniteStateMachine::CheckTransitionCodition(Transition& transition) 
{
    int conditionOrder = transition.Condition->ReflectFields->Order;
    if (nullptr == _nextState || conditionOrder < _nextOrder)
    {
        if (true == transition.Condition->Evaluate())
        {
            if (_currState != transition.NextState)
            {
                _nextState = transition.NextState;
                _nextOrder = conditionOrder;
            }
        }
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
            if (transition.CurrState == nullptr || transition.CurrState == _currState)
            {
                CheckTransitionCodition(transition);
            }
        }
    }

    if (nullptr != _nextState)
    {
        if (_currState)
        {
            _currState->OnExit();         
        }
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
