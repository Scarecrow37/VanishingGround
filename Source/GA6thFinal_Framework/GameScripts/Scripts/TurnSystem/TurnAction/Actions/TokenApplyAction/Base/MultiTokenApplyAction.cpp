#include "pchScripts.h"
#include "MultiTokenApplyAction.h"
#include "Token/TokenSystem.h"
#include "TurnSystem/TurnSystemHelper.h"
#include "MultiTokenApplyAction.h"

REFLECT_FUNCTION(MultiTokenApplyAction)

MultiTokenApplyAction::MultiTokenApplyAction() = default;

void MultiTokenApplyAction::ImGuiDrawPropertysEvent() 
{
    ImGui::Text("Action");
    if (ImGui::TreeNode((const char*)u8"추가 부여 토큰 ID"))
    {
        auto& plusID = ReflectFields->Tokens; 
        bool  erased = false;

        for (size_t i = 0; i < plusID.size(); ++i)
        {
            ImGui::PushID(static_cast<int>(i));
            ImGui::SetNextItemWidth(200);

            ImGui::InputInt("##A", &plusID[i].first);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200);
            ImGui::InputInt("##B", &plusID[i].second);

            ImGui::SameLine();
            if (ImGui::Button("X"))
            {
                plusID.erase(plusID.begin() + i);
                erased = true;
                ImGui::PopID();
                break; 
            }
            ImGui::PopID();
        }

        if (!erased && ImGui::Button("+ Add"))
        {
            plusID.emplace_back(0, 0);
        }
        UpdateActionInfo();
        ImGui::TreePop();
    }
    if (TurnSystemHelper::DrawTargetComboboxWithToolTip(ReflectFields->TokenTarget, true))
    {
        UpdateActionInfo();
    }
    ImGui::Separator();
    ImGui::Text("Conditions");
    ImguiDrawConditionEditor();
}

void MultiTokenApplyAction::TryTokenSystemInfoUpdate()
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