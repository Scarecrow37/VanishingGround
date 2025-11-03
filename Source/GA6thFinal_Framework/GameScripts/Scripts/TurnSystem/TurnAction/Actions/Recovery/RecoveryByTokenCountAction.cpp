#include "pchScripts.h"
#include "RecoveryByTokenCountAction.h"

#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "Token/TokenSystem.h"
#include "Token/TokenInventory.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "TurnSystem/TurnSystemHelper.h"

REFLECT_FUNCTION(RecoveryByTokenCountAction)
REGISTER_TURN_ACTION(RecoveryByTokenCountAction)

void RecoveryByTokenCountAction::OnEnemyDeadByWeapon(Enemy& enemy, WeaponElement& weapon) 
{
    if (EvaluateConditions())
    {
        TokenInventory& inven = enemy.GetTokenInventory();
        int count = inven.GetTokenStackFromID(ReflectFields->TokenID);
        if (0 < count)
        {
            auto targets = TurnSystemHelper::GetTargetCharacters(ReflectFields->Target);
            for (auto& target : targets)
            {
                if (target)
                {
                    target->Heal(count);
                }
            }
        }    
    }
}

const std::string& RecoveryByTokenCountAction::GetActionName()
{
    return NAME;
}

const std::string& RecoveryByTokenCountAction::GetActionInfo()
{
    return NAME;
}

void RecoveryByTokenCountAction::ImGuiDrawActionEditor() 
{
    ImGuiDrawPropertys();
    std::string_view prevValue = TokenSystem::TokenIDToName(ReflectFields->TokenID);
    if (prevValue.empty())
    {
        prevValue = STR_NULL;
    }
    if (ImGui::BeginCombo("Token ID##A72AE710-2115-4E9C-BC03-9709C4100F04", prevValue.data()))
    {
        for (auto& id : TokenSystem::GetRegisteredTokenList())
        {
            std::string_view name = TokenSystem::TokenIDToName(id);
            if (false == name.empty())
            {
                bool selectable = ReflectFields->TokenID == id;
                if (ImGui::Selectable(name.data(), selectable))
                {
                    ReflectFields->TokenID = id;
                }
            }
        }
        ImGui::EndCombo();
    }
    TurnSystemHelper::DrawTargetComboboxWithToolTip(ReflectFields->Target, true);
    ImguiDrawConditionEditor();
}
