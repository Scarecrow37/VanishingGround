#pragma once
#include "Monster/Common/MonsterCommon.h"
#include "MonsterAI.h"

class Enemy;

namespace Monster
{
    class AIFactory final
    {
        using AIBuildFunc = std::function<void(std::weak_ptr<Enemy>, AIController&)>;
    public:
        AIFactory()  = default;
        ~AIFactory() = default;

    public:
        static bool GetAIModel(FSMID id, std::weak_ptr<Enemy> weakOwner);
        static bool RegisterAIBuilder(FSMID id, AIBuildFunc builderFunc);

    private:
        static std::unordered_map<FSMID, AIBuildFunc> _aiBuilderTable;
    
    };
};
