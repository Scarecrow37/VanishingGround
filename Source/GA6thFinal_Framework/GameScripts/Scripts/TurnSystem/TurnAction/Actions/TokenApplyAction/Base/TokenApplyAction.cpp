#include "pchScripts.h"
#include "TokenApplyAction.h"
#include "Token/TokenSystem.h"
#include "TurnSystem/TurnSystemHelper.h"

REFLECT_FUNCTION(TokenApplyAction)

void TokenApplyAction::ImGuiDrawPropertysEvent()
{
    if (TokenSystem* tokenSystem = SingletonComponent<TokenSystem>::GetInstance())
    {
        ImGui::Text("Action");
        std::string_view prevValue = tokenSystem->GetTokenNameFromID(ReflectFields->TokenID);
        if (prevValue.empty())
        {
            prevValue = STR_NULL;
        }
        if (ImGui::BeginCombo("Token ID##A72AE710-2115-4E9C-BC03-9709C4100F04", prevValue.data()))
        {
            bool selectable = false;
            for (auto& token : tokenSystem->GetTokenInstances())
            {
                const int          id   = token->GetTokenID();
                const std::string& name = token->GetTokenName();
                selectable       = ReflectFields->TokenID == id ? true : false;
                if (ImGui::Selectable(name.c_str(), selectable))
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
}

void TokenApplyAction::DeserializedReflectEvent() 
{
    UpdateActionInfo();
}
