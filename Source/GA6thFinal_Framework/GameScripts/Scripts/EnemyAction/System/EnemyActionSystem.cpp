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
        ReflectFields->ActionTableSerialData.push_back(data.SerializedReflectFields());
    }
}

void EnemyActionSystem::DeserializedReflectEvent() 
{
    for (const auto& str : ReflectFields->ActionTableSerialData)
    {
        EnemyActionData data;
        data.DeserializedReflectFields(str);
        _enemyActionTable.emplace(data.ActionID, std::move(data));
    }
}

void EnemyActionSystem::ImGuiDrawPropertysEvent() 
{
    for (auto& [id, data] : _enemyActionTable)
    {
    }
}

const EnemyActionData& EnemyActionSystem::GetEnemyActionDataFromID(int actionID)
{
    auto itr = _enemyActionTable.find(actionID);
    if (itr != _enemyActionTable.end())
    {
        return itr->second;
    }
    return GetEmptyData();
}