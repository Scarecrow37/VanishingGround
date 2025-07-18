#include "pch.h"
#include "SpriteFontImporter.h"
#include "Editor/Tool/AssetBrowser/EditorAssetBrowserTool.h"

bool Importer::SpriteFontImporter::Initialize()
{
    // ttf와 otf 확장자를 가진 파일에 대하여 이벤트를 받습니다.
    UmFileSystem.RegisterFileEventSubscriber(this, {".ttf", ".otf"});
    return true;
}

void Importer::SpriteFontImporter::OnRequestedDragDrop(const File::Path& path)
{
    // 해당 경로가 지원하는 확장자인지 확인합니다.
    File::Path extension = path.extension();
    if (true == IsTriggerExtension(extension))
    {
        EditorModule* editor = Global::editorModule;
        if (editor)
        {
            auto* assetBrowser = EditorAssetBrowserTool::GetInstance();
            if (assetBrowser)
            {
                // AssetBrowser의 현재 포커스 폴더 경로를 가져옵니다.
                _importPath = assetBrowser->GetCurrentFocusFolderPath();
                // 팝업 박스 플래그
                int   flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
                // 팝업 박스 사이즈
                ImVec2 size = ImVec2(300, 400);
                // 임포트 세팅 팝업 박스를 엽니다.
                editor->OpenPopupBoxEx("SpriteFont Import", size, flags, [this]() { DrawImGuiImportSetting(); });
            }
        }
    }
}

// 텍스트와 프로퍼티 설정 위젯간에 일정한 패딩을 적용하여 세퍼레이터를 표시하기 위한 매크로입니다.
#define IMGUI_SEPARATOR_TEXT(text, offset)\
ImGui::Text(text);\
ImGui::SameLine(offset);\
ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);\
ImGui::SameLine();

void Importer::SpriteFontImporter::DrawImGuiImportSetting()
{
    std::string path = _importPath.string();
    ImGui::BeginDisabled();
    ImGui::InputText("##import path", &path, ImGuiInputTextFlags_ReadOnly); // 임포트할 폰트 경로를 표시합니다.
    if (ImGui::BeginItemTooltip()) // 호버링 시 툴팁으로 경로를 표시합니다.
    {
        ImGui::Text(path.c_str());
        ImGui::EndTooltip();
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    if (ImGui::Button(EditorIcon::ICON_FOLDER_OPEN)) // 경로 선택 버튼
    {
        File::ShowOpenFolderDialog(NULL, L"경로 선택", _importPath.c_str(), _importPath);
    }
    ImGui::Separator();

    ///////////////////////////////////////////////////////
    // 스프라이트 폰트 임포트 설정
    ///////////////////////////////////////////////////////
    if (ImGui::CollapsingHeader("Import Settings##sprite font importer", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const float separatorOffset = 130.0f; // 옵션 텍스트들의 일정한 패딩을 위한 오프셋

        // Font Size
        IMGUI_SEPARATOR_TEXT("Font Size", separatorOffset);
        ImGui::DragInt("##font size", &_fontSize, 1.0f);
        _fontSize = std::max(1, _fontSize); // Clamp

        // Font Color
        IMGUI_SEPARATOR_TEXT("Font Color", separatorOffset);
        ImGui::ColorEdit4("#font color", (float*)&_fontColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
      
    }
}
