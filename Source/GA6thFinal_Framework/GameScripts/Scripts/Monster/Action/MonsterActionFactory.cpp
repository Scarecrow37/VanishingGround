#include "pchScripts.h"
#include "MonsterActionFactory.h"

namespace Monster
{
    std::function<Action*()> ActionFactory::GetActionBuildFunc(ActionID id)
    {
        if (_actionBuilderTable.contains(id))
        {
            return _actionBuilderTable[id];
        }
        return nullptr;
    }
    
    bool ActionFactory::RegisterActionBuilder(ActionID id, ActionBuildFunc builderFunc)
    {
        if (builderFunc)
        {
            _actionBuilderTable[id] = builderFunc;
            return true;
        }
        return false;
    }
}
