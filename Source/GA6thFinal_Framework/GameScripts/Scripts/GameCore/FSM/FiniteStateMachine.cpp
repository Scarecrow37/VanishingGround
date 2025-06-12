#include "FiniteStateMachine.h"

FiniteStateMachine::FiniteStateMachine() = default;
FiniteStateMachine::~FiniteStateMachine() = default;

void FiniteStateMachine::SerializedReflectEvent() 
{
    for (auto& [key, state] : _stateMap)
    {
        ReflectFields->StateReflectDatas[key] = state->SerializedReflectFields();
    }
}

void FiniteStateMachine::DeserializedReflectEvent() 
{
    for (auto& [key, data] : ReflectFields->StateReflectDatas)
    {
        FSMState* state = NewInstanceWithKey(key);
        if (state)
        {
            _stateMap[key].reset(state);
            _stateMap[key]->DeserializedReflectFields(data);
        }
    }
}

void FiniteStateMachine::ImGuiDrawPropertysEvent() 
{
    const char* addKey = nullptr;
    for (auto& [key, func] : GetInstanceConstructors())
    {
        if (ImGui::Button(key.c_str()))
        {
            addKey = key.c_str();
        }
    }
    if (addKey)
    {
        AddState(addKey);
    }
}
