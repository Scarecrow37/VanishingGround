#include "pchScripts.h"
#include "TokenApplyAction.h"
#include "Token/TokenSystem.h"
#include "TurnSystem/TurnSystemHelper.h"

REFLECT_FUNCTION(TokenApplyAction)

TokenApplyAction::TokenApplyAction() 
{
    UpdateActionInfo();
}

void TokenApplyAction::ImGuiDrawPropertysEvent()
{
    ImGui::Text("Action");
    std::string_view prevValue = TokenSystem::GetTokenNameFromID(ReflectFields->TokenID);
    if (prevValue.empty())
    {
        prevValue = STR_NULL;
    }
    if (ImGui::BeginCombo("Token ID##A72AE710-2115-4E9C-BC03-9709C4100F04", prevValue.data()))
    {
        bool selectable = false;
        for (auto& token : TokenSystem::GetTokenInstances())
        {
            const char* name = token->GetTokenName();
            int         id   = token->GetTokenID();
            selectable       = ReflectFields->TokenID == id ? true : false;
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

void TokenApplyAction::DeserializedReflectEvent() 
{
    UpdateActionInfo();
}
