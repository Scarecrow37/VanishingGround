#include "pchScripts.h"
#include "DifficultyManager.h"

UMREAL_COMPONENT(DifficultyManager)

DifficultyManager::DifficultyManager() = default;

void DifficultyManager::SetDifficulty(const Difficulty difficulty)
{
    _defaultDifficulty = difficulty;
}

Difficulty DifficultyManager::GetDifficulty() const
{
    return _defaultDifficulty;
}

void DifficultyManager::Awake()
{
    Component::Awake();

    if (_singletonObject.TrySingleTon(true))
    {
        _singletonComponent.TrySingleTon();
    }
}