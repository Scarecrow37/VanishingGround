#include "pchScripts.h"
#include "CharacterStats.h"
#include "CharacterStatsComponent.h"

CharacterStatsComponent::CharacterStatsComponent() = default;
CharacterStatsComponent::~CharacterStatsComponent() = default;

void CharacterStatsComponent::SerializedReflectEvent() 
{
    ReflectFields->StatsData = _stats->SerializedReflectFields();
}

void CharacterStatsComponent::DeserializedReflectEvent() 
{
    _stats->DeserializedReflectFields(ReflectFields->StatsData);
}

void CharacterStatsComponent::ImGuiDrawPropertysEvent() 
{
    _stats->ImGuiDrawPropertys();
}
