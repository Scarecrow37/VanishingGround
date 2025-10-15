#pragma once
#include "Monster/Common/MonsterCommon.h"

namespace Monster
{
    class Action;

#define REGISTER_ACTION(ID)                                                                             \
    namespace ActionBuilder##ID                                                                         \
    {                                                                                                   \
        bool registered = ActionFactory::RegisterActionBuilder(ID, &BuildAction##ID);                   \
    }
    class ActionFactory
    {
        using ActionBuildFunc = std::function<Action*()>;
    public:
        ActionFactory()  = default;
        ~ActionFactory() = default;

    public:
        static ActionBuildFunc GetActionBuildFunc(ActionID id);
        static bool RegisterActionBuilder(ActionID id, ActionBuildFunc builderFunc);
    private:
        static std::unordered_map<Monster::ActionID, ActionBuildFunc> _actionBuilderTable;
    };
};
