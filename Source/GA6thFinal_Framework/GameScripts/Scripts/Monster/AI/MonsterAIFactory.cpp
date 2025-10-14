#include "pchScripts.h"
#include "MonsterAIFactory.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

namespace Monster
{
    bool AIFactory::GetAIModel(FSMID id, std::weak_ptr<Enemy> weakOwner)
    {
        if (_aiBuilderTable.contains(id) && false == weakOwner.expired())
        {
            if (auto owner = weakOwner.lock())
            {
                Monster::AIController& aiController = owner->GetAIController();
                aiController.Clear();
                _aiBuilderTable[id](owner, aiController);
                return true;
            }
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
