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

void Impoerter::SpriteFontImporter::DrawImGuiImportSetting() 
{
    std::string path = _importPath.string();
    ImGui::BeginDisabled();
    ImGui::InputText("##ImportPath", &path, ImGuiInputTextFlags_ReadOnly);
    ImGui::EndDisabled();
    ImGui::SameLine();
    if (ImGui::Button(EditorIcon::ICON_FOLDER_OPEN))
    {
        File::ShowOpenFolderDialog(NULL, L"경로 선택", _importPath.c_str(), _importPath);
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Import Settings##sprite font importer", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Font Import Settings");
        // 여기에 폰트 임포트 설정을 추가할 수 있습니다.
        // 예: 폰트 크기, 스타일 등
        ImGui::InputInt("Font Size", &_fontSize);
        ImGui::ColorEdit4("Font Color", (float*)&_fontColor,
                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    }
}
