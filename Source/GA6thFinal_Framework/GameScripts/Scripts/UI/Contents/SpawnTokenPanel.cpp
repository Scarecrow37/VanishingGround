#include "pchScripts.h"
#include "SpawnTokenPanel.h"

UMREAL_COMPONENT(SpawnTokenPanel)

SpawnTokenPanel::SpawnTokenPanel() : _newColor(DEFAULT_COLOR)
{
    FilePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".png")
                {
                    _Guid               = data->GetGuid();
                    ReflectFields->Guid = _Guid.string();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

SIZE SpawnTokenPanel::MeasureOverride(const SIZE availableSize)
{
    return availableSize;
}

SIZE SpawnTokenPanel::ArrangeOverride(const SIZE finalSize)
{
    return finalSize;
}

namespace
{
    struct Row
    {
        void operator()(const int tokenID, std::array<float, 4>& color, const std::function<void()>& deleteCallback) const
        {
            // ID
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", tokenID);

            // Color
            ImGui::TableSetColumnIndex(1);
            float editColor[4];
            std::ranges::copy(color, editColor);
            if (ImGui::ColorEdit4("##Color", editColor))
            {
                std::ranges::copy(editColor, color.begin());
            }

            // Delete button
            const float height = ImGui::GetItemRectSize().y;
            ImGui::SameLine();
            if (ImGui::Button("-", ImVec2(height, height)))
            {
                deleteCallback();
            }
        }

        void operator()(int* newID, std::array<float, 4>* newColor, const bool isExisted,
                        const std::function<void()>& addCallback) const
        {
            if (newID == nullptr || newColor == nullptr)
            {
                return;
            }

            // ID
            ImGui::TableSetColumnIndex(0);
            ImVec2 availSize = ImGui::GetContentRegionAvail();
            ImGui::SetNextItemWidth(availSize.x);
            ImGuiHelper::StyleBuilder style;
            if (isExisted)
                style.PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.15f, 0.25f, 1.0f));
            else
                style.PushStyleColor(ImGuiCol_Text, ImVec4(0.15f, 0.75f, 0.35f, 1.0f));
            ImGui::InputInt("##NewKey", newID, 0);
            style.PopStyle();

            // Color
            ImGui::TableSetColumnIndex(1);
            availSize = ImGui::GetContentRegionAvail();
            ImGui::SetNextItemWidth(availSize.x - 60.0f);
            constexpr int flags = ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_EnterReturnsTrue;
            float         editColor[4];
            std::ranges::copy(*newColor, editColor);
            if (ImGui::ColorEdit4("##Color", editColor))
            {
                std::ranges::copy(editColor, newColor->begin());
            }

            // Add Button
            const float height = ImGui::GetItemRectSize().y;
            ImGui::SameLine();
            if (!isExisted && ImGui::Button("+", ImVec2(height, height)))
            {
                addCallback();
            }
        }
    };

    struct EraseLater
    {
        using Map = std::unordered_map<int, std::array<float, 4>>;
        explicit EraseLater(Map* map) : MapPointer(map) {}
        EraseLater(const EraseLater&)                = delete;
        EraseLater& operator=(const EraseLater&)     = delete;
        EraseLater(EraseLater&&) noexcept            = delete;
        EraseLater& operator=(EraseLater&&) noexcept = delete;
        ~EraseLater()
        {
            for (const auto& key : KeysToErase)
            {
                MapPointer->erase(key);
            }
            KeysToErase.clear();
        }

        void operator()(const int key) { KeysToErase.push_back(key); }

        Map*             MapPointer;
        std::vector<int> KeysToErase;
    };
} // namespace

void SpawnTokenPanel::ImGuiDrawPropertysEvent()
{
    UIComponent::ImGuiDrawPropertysEvent();

    auto& tokenColors = ReflectFields->TokenColors;
    if (ImGui::TreeNodeEx("Token Colors##details"))
    {
        // AddMapping Function
        if (ImGui::BeginTable("ColorTable##Details", 2, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg))
        {
            // Headers
            ImGui::TableSetupColumn("Token ID", ImGuiTableColumnFlags_WidthStretch, 0.3f);
            ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthStretch, 0.6f);
            ImGui::TableHeadersRow();

            // Existing Colors
            EraseLater eraseLater(&tokenColors);
            for (auto& [tokenID, colorArray] : tokenColors)
            {
                ImGui::PushID(tokenID);
                ImGui::TableNextRow();
                Row()(tokenID, colorArray, [tokenID, &eraseLater]() { eraseLater(tokenID); });
                ImGui::PopID();
            }

            // New Color;
            ImGui::TableNextRow();
            Row()(&_newID, &_newColor, tokenColors.contains(_newID), [this, &tokenColors]() {
                if (auto [iterator, isSucceed] = tokenColors.try_emplace(_newID, _newColor); isSucceed)
                {
                    _newID = 0;
                    _newColor = DEFAULT_COLOR;
                }
            });
            ImGui::EndTable();
        }

        ImGui::TreePop();
    }
}

