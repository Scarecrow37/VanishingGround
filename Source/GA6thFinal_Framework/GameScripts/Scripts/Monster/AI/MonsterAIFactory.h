#pragma once
#include "Monster/Common/MonsterCommon.h"
#include "MonsterAIModel.h"

class Enemy;

namespace Monster
{
    class AIFactory final
    {
        using AIBuildFunc = std::function<void(std::weak_ptr<Enemy>, AIModel&)>;

    public:
        AIFactory();
        ~AIFactory();

    public:
        static AIBuildFunc GetAIBuildFunc(FSMID id);
        static bool RegisterAIBuilder(FSMID id, AIBuildFunc builderFunc);

    private:
        inline static std::unordered_map<FSMID, AIBuildFunc> _aiBuilderTable;
    
    };
};
