#include "pch.h"
#include "SpriteFontImporter.h"
#include "Editor/Tool/AssetBrowser/EditorAssetBrowserTool.h"

bool Impoerter::SpriteFontImporter::Initialize()
{
    UmFileSystem.RegisterFileEventSubscriber(this, {".ttf", ".otf"});
    return true;
}

void Impoerter::SpriteFontImporter::OnRequestedDragDrop(const File::Path& path) 
{
    File::Path extension = path.extension();
    if (true == IsTriggerExtension(extension))
    {
        EditorModule* editor = Global::editorModule;
        if (editor)
        {
            auto* assetBrowser = EditorAssetBrowserTool::GetInstance();
            if (assetBrowser)
            {
                _importPath  = assetBrowser->GetCurrentFocusFolderPath();
                int    flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
                ImVec2 size  = ImVec2(300, 400);

                editor->OpenPopupBoxEx("SpriteFont Import", size, flags, [this]() { DrawImGuiImportSetting(); });
            }
        }
    }
}

#define IMGUI_TEXT_BLOCK(text, offset)\
ImGui::Text("Font Size");\
ImGui::SameLine(offset);\
ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);\
ImGui::SameLine();

void Impoerter::SpriteFontImporter::DrawImGuiImportSetting() 
{
    std::string path = _importPath.string();
    ImGui::BeginDisabled();
    ImGui::InputText("##import path", &path, ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginItemTooltip())
    {
        ImGui::Text(path.c_str());
        ImGui::EndTooltip();
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    if (ImGui::Button(EditorIcon::ICON_FOLDER_OPEN))
    {
        File::ShowOpenFolderDialog(NULL, L"경로 선택", _importPath.c_str(), _importPath);
    }
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Import Settings##sprite font importer", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const float separatorOffset = 130.0f;
        IMGUI_TEXT_BLOCK("Font Size", separatorOffset);
        ImGui::DragInt("##font size", &_fontSize, 1.0f);
        _fontSize = std::max(1, _fontSize); // Clamp

        IMGUI_TEXT_BLOCK("Font Color", separatorOffset);
        ImGui::ColorEdit4("#font color", (float*)&_fontColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
      
    }
}
