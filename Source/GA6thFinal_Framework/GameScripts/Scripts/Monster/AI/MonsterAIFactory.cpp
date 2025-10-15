#include "pchScripts.h"
#include "MonsterAIFactory.h"

namespace Monster
{
    AIFactory::AIFactory() = default;
    AIFactory::~AIFactory() = default;

    std::function<void(std::weak_ptr<Enemy>, AIModel&)> AIFactory::GetAIBuildFunc(FSMID id)
    {
        if (_aiBuilderTable.contains(id))
        {
            return _aiBuilderTable[id];
        }
        return nullptr;
    }
    bool AIFactory::RegisterAIBuilder(FSMID id, AIBuildFunc builderFunc) 
    {
        if (!_aiBuilderTable.contains(id))
        {
            _aiBuilderTable[id] = builderFunc;
            return true;
        }
        return false;
    }
}
