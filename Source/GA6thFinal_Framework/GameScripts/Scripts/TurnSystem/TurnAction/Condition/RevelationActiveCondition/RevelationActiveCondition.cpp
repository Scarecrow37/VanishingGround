#include "pchScripts.h"
#include "RevelationActiveCondition.h"
#include "TurnSystem/TurnMode/TurnMode.h"

REGISTER_TURN_ACTION_CONDITION(RevelationActiveCondition)

RevelationActiveCondition::RevelationActiveCondition()  = default;
RevelationActiveCondition::~RevelationActiveCondition() = default;

bool RevelationActiveCondition::Evaluate()
{
    if (TurnMode* system = SingletonComponent<TurnMode>::GetInstance())
    {
        return true == system->RevelationActiveFlag;
    }
    return false;
}

void RevelationActiveCondition::DrawImguiEditor() 
{
    using namespace u8_literals;
    ImGui::Text( u8"계시 발동시"_c_str);
}

const std::string& RevelationActiveCondition::GetConditionInfo()
{
    using namespace u8_literals;
    static const std::string info = u8"계시 발동시"_c_str;
    return info;
}

bool RevelationActiveCondition::CheckEvaluate(CharacterBase* character)
{
    return false;
}


