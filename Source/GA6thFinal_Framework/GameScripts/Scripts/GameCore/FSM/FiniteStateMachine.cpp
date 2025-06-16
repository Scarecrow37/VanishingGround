#include "FiniteStateMachine.h"

FiniteStateMachine::FiniteStateMachine()  = default;
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

const char* FiniteStateMachine::AddStateImguiPopUp() 
{
    const char* addKey = nullptr;
    if (ImGui::BeginChild("ConstructorsChild", ImVec2(0, 100), ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_AlwaysVerticalScrollbar))
    {
        static ImGuiTextFilter filter;
        filter.Draw("Search");
        for (auto& [key, func] : GetInstanceConstructors())
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
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
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
    }
    ImGui::PopID();
}
