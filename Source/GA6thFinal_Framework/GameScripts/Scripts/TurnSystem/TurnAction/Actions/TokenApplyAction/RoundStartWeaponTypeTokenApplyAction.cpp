#include "pchScripts.h"
#include "RoundStartWeaponTypeTokenApplyAction.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "Token/TokenSystem.h"
#include "TurnSystem/TurnSystemHelper.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "WeaponSystem/WeaponSystem.h"

REGISTER_TURN_ACTION(RoundStartWeaponTypeTokenApplyAction)
REFLECT_FUNCTION(RoundStartWeaponTypeTokenApplyAction)

const std::string& RoundStartWeaponTypeTokenApplyAction::GetActionInfo()
{
    TryTokenSystemInfoUpdate();
    return NAME;
}

void RoundStartWeaponTypeTokenApplyAction::ImGuiDrawActionEditor()
{
    ImGuiDrawPropertys();
}

const std::string& RoundStartWeaponTypeTokenApplyAction::GetActionName()
{
    return NAME;
}

void RoundStartWeaponTypeTokenApplyAction::OnRoundStart()
{
    if (EvaluateConditions())
    {
        if (TokenSystem* tokenSystem = SingletonComponent<TokenSystem>::GetInstance())
        {
            std::vector<CharacterBase*> targets = TurnSystemHelper::GetTargetCharacters(Target);
            int tokenCount = TokenCount;
            int typeCount = 0;
            if (WeaponSystem* system = SingletonComponent<WeaponSystem>::GetInstance())
            {
                typeCount = system->GetEquipWeaponTypeCount(ReflectFields->Type);
            }

            int applyCount = tokenCount * typeCount;
            if (0 < applyCount)
            {
                if (false == targets.empty())
                {
                    for (auto& target : targets)
                    {
                        TokenInventory& tokenInventory = target->GetTokenInventory();
                        tokenInventory.AddTokenStackFromID(TokenID, applyCount);
                    }
                }
            }
        }
    }
}