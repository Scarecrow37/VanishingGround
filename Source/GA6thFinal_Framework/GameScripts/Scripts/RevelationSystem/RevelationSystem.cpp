#include "pchScripts.h"
#include "RevelationSystem.h"
RevelationSystem::RevelationSystem() = default;
RevelationSystem::~RevelationSystem() = default;

void RevelationSystem::SerializedReflectEvent() 
{
    ReflectFields->RevelationActionDatas.clear();
    for (auto& [key, action] : _actions)
    {
        std::string data = action->SerializedReflectFields();
        ReflectFields->RevelationActionDatas.emplace_back(key, data);
    }
}

void RevelationSystem::DeserializedReflectEvent() 
{
    _actions.clear();
    for (int i = 0; i < ReflectFields->RevelationActionDatas.size(); ++i)
    {
        auto& [key, data] = ReflectFields->RevelationActionDatas[i];
        RevelationActionBase* action = _actions[key].get();
        action->DeserializedReflectFields(data);
    }
}

void RevelationSystem::ImGuiDrawPropertysEvent() 
{
    if (ImGui::BeginCombo("Actions", "a"))
    {
        ImGui::EndCombo();
    }
}

void RevelationSystem::Reset()
{
    static_instance = this;

    _actions.clear();
    const auto& construtors = GetInstanceConstructors();
    for (auto& [key, func] : construtors)
    {
        _actions[key].reset(func());
    }
}
