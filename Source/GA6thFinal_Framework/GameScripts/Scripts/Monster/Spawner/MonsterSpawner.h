#pragma once
#include "Monster/Context/MonsterSpawnContext.h"

namespace Monster
{
    class Spawner
    {
    public:
        Spawner();
        ~Spawner();

    public:


    private:
        std::unordered_map<LevelID, MonsterSpawnContext> _spawnContextMap;
    };
}