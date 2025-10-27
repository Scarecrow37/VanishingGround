#include "pchScripts.h"
#include "RevelationActiveCondition.h"
#include "RevelationSystem/RevelationSystem.h"

REGISTER_TURN_ACTION_CONDITION(RevelationActiveCondition)

RevelationActiveCondition::RevelationActiveCondition()
{
    UpdateInfo();
}
RevelationActiveCondition::~RevelationActiveCondition() = default;

bool RevelationActiveCondition::Evaluate()
{
    if (RevelationSystem* system = SingletonComponent<RevelationSystem>::GetInstance())
    {
        auto& roundRevelations = system->GetRoundElementList();
        if (false == roundRevelations.empty())
        {
            size_t i = 0;
            for (auto& revelation : roundRevelations)
            {
                if (system->IsCurrentTurnRevelationActive(i))
                {
                    return true;
                }
            }
        }
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
    return _info;
}

void RevelationActiveCondition::UpdateInfo() 
{
    using namespace u8_literals;
    _info.clear();
    _info = u8"계시 발동시"_c_str;
}

