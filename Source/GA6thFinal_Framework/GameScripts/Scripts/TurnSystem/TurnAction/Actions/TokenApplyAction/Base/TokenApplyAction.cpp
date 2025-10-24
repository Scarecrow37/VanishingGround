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
            const char* name = TokenSystem::TokenIDToName(id);
            name == "" ? STR_NULL : name;
            bool selectable = ReflectFields->TokenID == id ? true : false;
            if (ImGui::Selectable(name, selectable))
            {
                TokenID = id;
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
            UpdateActionInfo();
            validTokenSystem = true;
        }     
    }
}
