#include "pchScripts.h"
#include "TokenDamageAction.h"

#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "Token/TokenSystem.h"
#include "TurnSystem/TurnSystemHelper.h"

REFLECT_FUNCTION(TokenDamaggeAction)
REGISTER_TURN_ACTION(TokenDamaggeAction)

const std::string& TokenDamaggeAction::GetActionName()
{
    return NAME;
}

const std::string& TokenDamaggeAction::GetActionInfo()
{
    return NAME;
}

void TokenDamaggeAction::ImGuiDrawActionEditor() 
{
    ImGuiDrawPropertys();
}

void TokenDamaggeAction::ImGuiDrawPropertysEvent()
{
    using namespace u8_literals;

    ImGui::Text("Action");
    std::string_view prevValue = TokenSystem::TokenIDToName(ReflectFields->TokenID);
    if (prevValue.empty())
    {
        prevValue = STR_NULL;
    }
    if (ImGui::BeginCombo("Token ID##A72AE710-2115-4E9C-BC03-9709C4100F04", prevValue.data()))
    {
        bool selectable = ReflectFields->TokenID == TokenObject::Bleed::ID;
        if (ImGui::Selectable(u8"출혈"_c_str, selectable))
        {
            TokenID = TokenObject::Bleed::ID;
        }
        selectable = ReflectFields->TokenID == TokenObject::Poison::ID;
        if (ImGui::Selectable(u8"중독"_c_str, selectable))
        {
            TokenID = TokenObject::Poison::ID;
        }
        ImGui::EndCombo();
    }
    ImGui::Separator();
    ImGui::Text("Conditions");
    ImguiDrawConditionEditor();
}

void TokenDamaggeAction::TryTokenSystemInfoUpdate()
{
    if (false == validTokenSystem)
    {
        if (TokenSystem* system = SingletonComponent<TokenSystem>::GetInstance())
        {
            const std::string& name = system->GetTokenNameFromID(TokenID);
            if (name.empty())
            {
                ReflectFields->TokenID = TokenObject::Bleed::ID;
            }
            validTokenSystem = true;
        }
    }
}
