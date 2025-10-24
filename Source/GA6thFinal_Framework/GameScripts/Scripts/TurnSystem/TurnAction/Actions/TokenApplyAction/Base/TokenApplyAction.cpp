#include "pchScripts.h"
#include "TokenApplyAction.h"
#include "Token/TokenSystem.h"
#include "TurnSystem/TurnSystemHelper.h"

TokenApplyAction::TokenApplyAction() 
{
    ReflectFields->TokenID = TokenObject::Bleed::ID;
}

REFLECT_FUNCTION(TokenApplyAction)

void TokenApplyAction::ImGuiDrawPropertysEvent()
{
    ImGui::Text("Action");
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
                    TokenID = id;
                }
            }
        }
        ImGui::EndCombo();
    }
    ReflectHelper::ImGuiDraw::Private::InputAuto(TokenCount, UmCore->ImGuiDrawPropertysSetting);
    if (TurnSystemHelper::DrawTargetComboboxWithToolTip(ReflectFields->TokenTarget, true))
    {
        UpdateActionInfo();
    }
    ImGui::Separator();
    ImGui::Text("Conditions");
    ImguiDrawConditionEditor();
}

void TokenApplyAction::TryTokenSystemInfoUpdate() 
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
            UpdateActionInfo();
            validTokenSystem = true;
        }     
    }
}
