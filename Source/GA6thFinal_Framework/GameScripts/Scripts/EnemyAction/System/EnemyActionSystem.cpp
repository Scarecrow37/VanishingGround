#include "pchScripts.h"
#include "EnemyActionSystem.h"

EnemyActionSystem::EnemyActionSystem() {}

EnemyActionSystem::~EnemyActionSystem()
{
    if (this == _staticInstance)
    {
        _staticInstance = nullptr;
    }
}

void EnemyActionSystem::Reset() 
{
    _staticInstance = this;
}

void EnemyActionSystem::SerializedReflectEvent() 
{
    for (auto& [id, data] : _enemyActionTable)
    {
        if (data)
        {
            ReflectFields->ActionSerializeDataTable[id] = data->SerializedReflectFields();
        }
    }
}

void EnemyActionSystem::DeserializedReflectEvent() 
{
    for (const auto& [id, str] : ReflectFields->ActionSerializeDataTable)
    {
        EnemyAction::ActionData* data = new EnemyAction::ActionData;
        data->DeserializedReflectFields(str);
        _enemyActionTable[id] = data;
    }
}

void EnemyActionSystem::ImGuiDrawPropertysEvent() 
{
    for (auto& [id, data] : _enemyActionTable)
    {
    }
}

const EnemyAction::ActionData* EnemyActionSystem::GetEnemyActionDataFromID(int actionID)
{
    auto itr = _enemyActionTable.find(actionID);
    if (itr != _enemyActionTable.end())
    {
        return itr->second;
    }
    return nullptr;
}