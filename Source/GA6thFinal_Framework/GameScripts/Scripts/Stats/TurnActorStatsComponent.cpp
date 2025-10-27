#include "pchScripts.h"
#include "CharacterStats.h"
#include "TurnActorStatsComponent.h"

REFLECT_FUNCTION(TurnActorStatsComponent)

TurnActorStatsComponent::TurnActorStatsComponent() = default;
TurnActorStatsComponent::~TurnActorStatsComponent() = default;

void TurnActorStatsComponent::SerializedReflectEvent() 
{
    ReflectFields->StatsData = _stats->SerializedReflectFields();
}

void TurnActorStatsComponent::DeserializedReflectEvent() 
{
    if (nullptr == _stats)
    {
        TurnActorStats* newStats = NewTurnActorStats();
        _stats.reset(newStats);
    }
    _stats->DeserializedReflectFields(ReflectFields->StatsData);
}

void TurnActorStatsComponent::ImGuiDrawPropertysEvent() 
{
    _stats->ImGuiDrawPropertys();
}

void TurnActorStatsComponent::Reset()
{
    if (nullptr == _stats)
    {
        TurnActorStats* newStats = NewTurnActorStats();
        _stats.reset(newStats);
    }
}