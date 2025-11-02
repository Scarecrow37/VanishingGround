#include "pchScripts.h"
#include "TokenGrantorAction.h"
#include "TurnSystem/TurnSystemHelper.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"

REGISTER_TURN_ACTION(TokenGrantorAction)

const std::string& TokenGrantorAction::GetActionName()
{
    return NAME;
}

const std::string& TokenGrantorAction::GetActionInfo()
{
    return NAME;
}

void TokenGrantorAction::ImGuiDrawActionEditor() 
{
    ImGuiDrawPropertys();
}

void TokenGrantorAction::OnTokenAddedStart(CharacterBase& targetCharacter, int& tokenID, int& tokenCount) 
{
    if (ReflectFields->TokenID == tokenID)
    {
        auto targets = TurnSystemHelper::GetTargetCharacters(ReflectFields->TokenTarget);
        for (auto& target : targets)
        {
            if (target == &targetCharacter)
            {
                if (EvaluateConditions())
                {
                    tokenCount += ReflectFields->TokenCount;
                }
                break;
            }
        }     
    }   
}
