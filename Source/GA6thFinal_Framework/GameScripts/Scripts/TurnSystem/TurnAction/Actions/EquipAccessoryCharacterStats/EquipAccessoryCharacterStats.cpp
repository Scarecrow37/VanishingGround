#include "pchScripts.h"
#include "EquipAccessoryCharacterStats.h"
#include "TurnSystem/TurnSystemHelper.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "PlayerSystem/PlayerSystem.h"
#include "Stats/Player/PlayerStats.h"
#include "Stats/Player/PlayerStatsComponent.h"

REFLECT_FUNCTION(EquipAccessoryCharacterStats)

REGISTER_TURN_ACTION(EquipAccessoryCharacterStats)

const std::string& EquipAccessoryCharacterStats::GetActionName()
{  
    return ACTION_NAME;
}

const std::string& EquipAccessoryCharacterStats::GetActionInfo()
{
    return ACTION_NAME;
}

void EquipAccessoryCharacterStats::ImGuiDrawActionEditor() 
{
    ImGuiDrawPropertys();
    ImguiDrawConditionEditor();
}

void EquipAccessoryCharacterStats::OnEquipAccessory() 
{
    if (PlayerSystem* system = SingletonComponent<PlayerSystem>::GetInstance())
    {
        if (EvaluateConditions())
        {
            auto targets = TurnSystemHelper::GetTargetCharacters(ReflectFields->Target);
            for (auto& target : targets)
            {
                if (target)
                {
                    if (CharacterStats* stats = target->GetCharacterStats())
                    {
                        int maxHp              = ReflectFields->MaxHP;
                        int currentHp          = ReflectFields->CurrentHP;
                        int stunResistance     = ReflectFields->StunResistance;

                        stats->MaxHP += maxHp;
                        stats->CurrentHP += currentHp;
                        stats->StunResistance += stunResistance;
                    }   
                }
            }
        }
    }
}

void EquipAccessoryCharacterStats::OnCharacterMaxChainRoundCountUse(CharacterBase& character, int& maxChainRoundCount)
{
    if (EvaluateConditions())
    {
        auto targets = TurnSystemHelper::GetTargetCharacters(ReflectFields->Target);
        for (auto& target : targets)
        {
            if (target == &character)
            {
                maxChainRoundCount += ReflectFields->MaxChainRoundCount;
                break;
            }
        }
    }
}
