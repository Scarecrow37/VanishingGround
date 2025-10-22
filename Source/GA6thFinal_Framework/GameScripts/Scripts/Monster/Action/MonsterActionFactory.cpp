#include "pchScripts.h"
#include "MonsterActionFactory.h"
#include "MonsterActionBase.h"

namespace Monster
{
    ActionFactory::ActionFactory() = default;
    ActionFactory::~ActionFactory() = default;

    bool ActionFactory::NewActionFromID(ActionID id, Action::Base** outAction)
    {
        if (_actionBuilderTable.contains(id))
        {
            if (_actionBuilderTable[id])
            {
                *outAction = _actionBuilderTable[id]();
                return true;
            }
        }
        return false;
    }
}
