#include "pchScripts.h"
#include "MonsterAIFactory.h"

namespace Monster
{
    bool AIFactory::GetAIModel(FSMID id, std::weak_ptr<Enemy> owner, AIController& controller)
    {
        if (_aiBuilderTable.contains(id))
        {
            controller.Clear();
            _aiBuilderTable[id](owner, controller);
            return true;
        }
        return false;
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
