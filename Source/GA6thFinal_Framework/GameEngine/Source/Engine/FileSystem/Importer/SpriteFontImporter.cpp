#include "pch.h"
#include "SpriteFontImporter.h"
#include "Editor/Tool/AssetBrowser/EditorAssetBrowserTool.h"

Importer::SpriteFontImporter::~SpriteFontImporter()
{
    if (nullptr != _moduleHandle)
        FreeLibrary(_moduleHandle);
}

void Importer::SpriteFontImporter::Initialize()
{
    // ttf와 otf, spritefont확장자를 가진 파일에 대하여 이벤트를 받습니다.
    UmFileSystem.RegisterFileEventSubscriber(this, {".ttf", ".otf", ".spritefont"});

    // MakeSpriteFont DLL 로드
    _moduleHandle = LoadLibrary(L"MakeSpriteFontCLI.dll");
    if (nullptr == _moduleHandle)
    {
        const std::string errorMessage = GetMessageFromLastError()();
        throw std::runtime_error("Failed to load MakeSpriteFontCLI.dll: " + errorMessage);
    }

    // MakeSpriteFont 프로시져 가져오기
    _makeSpriteFont = reinterpret_cast<MakeSpriteFont>(GetProcAddress(_moduleHandle, "MakeSpriteFont"));
    if (nullptr == _makeSpriteFont)
    {
        const std::string errorMessage = GetMessageFromLastError()();
        throw std::runtime_error("Failed to get MakeSpriteFont function: " + errorMessage);
    }
}

void Importer::SpriteFontImporter::OnRequestedDragDrop(const File::Path& path)
{
    // 해당 경로가 지원하는 확장자인지 확인합니다.
    File::Path extension = path.extension();
    if (true == IsTriggerExtension(extension))
    {
        _sourcePath          = path;
        EditorModule* editor = Global::editorModule;
        if (editor)
        {
            auto* assetBrowser = EditorAssetBrowserTool::GetInstance();
            if (assetBrowser)
            {
                // AssetBrowser의 현재 포커스 폴더 경로를 가져옵니다.
                _importPath = assetBrowser->GetCurrentFocusFolderPath();

                // 팝업 박스 플래그
                int flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
                // 팝업 박스 사이즈
                ImVec2 size = ImVec2(300, 400);
                // 임포트 세팅 팝업 박스를 엽니다.
                editor->OpenPopupBoxEx("SpriteFont Import", size, flags, [this]() { DrawImGuiImportSetting(); });
            }
        }
    }
}

// 텍스트와 프로퍼티 설정 위젯간에 일정한 패딩을 적용하여 세퍼레이터를 표시하기 위한 매크로입니다.
#define IMGUI_SEPARATOR_TEXT(text, offset)                                                                             \
    ImGui::Text(text);                                                                                                 \
    ImGui::SameLine(offset);                                                                                           \
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);                                                                  \
    ImGui::SameLine();

void Importer::SpriteFontImporter::DrawImGuiImportSetting()
{
    std::string importPath = _importPath.string();
    ImGui::BeginDisabled();
    ImGui::InputText("##import path", &importPath, ImGuiInputTextFlags_ReadOnly); // 임포트할 폰트 경로를 표시합니다.
    if (ImGui::BeginItemTooltip()) // 호버링 시 툴팁으로 경로를 표시합니다.
    {
        ImGui::Text(importPath.c_str());
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
        constexpr float separatorOffset = 130.0f; // 옵션 텍스트들의 일정한 패딩을 위한 오프셋

        // Source Path
        std::string sourcePath = _sourcePath.string();
        IMGUI_SEPARATOR_TEXT("Source Path", separatorOffset)
        ImGui::BeginDisabled();
        ImGui::InputText("##Source path", &sourcePath, ImGuiInputTextFlags_ReadOnly);
        if (ImGui::BeginItemTooltip())
        {
            ImGui::Text(sourcePath.c_str());
            ImGui::EndTooltip();
        }
        ImGui::EndDisabled();

        // File Name
        std::string fileName = _sourcePath.filename().string();
        IMGUI_SEPARATOR_TEXT("File Name", separatorOffset)
        ImGui::BeginDisabled();
        ImGui::InputText("##File name", &fileName, ImGuiInputTextFlags_ReadOnly);
        ImGui::EndDisabled();
        ImGui::TextColored(ImVec4{1.0f, 0.0f, 0.0f, 1.0f},
                           reinterpret_cast<const char*>(u8"파일 이름은 폰트 이름과 동일해야 합니다."));

        // Default Character
        IMGUI_SEPARATOR_TEXT("Default Character", separatorOffset);
        ImGui::InputText("##Default Character", &_defaultCharacter, 1);
        if (ImGui::BeginItemTooltip())
        {
            ImGui::Text(reinterpret_cast<const char*>(u8"생성된 폰트에 문자가 없을 경우 대신 출력될 문자"));
            ImGui::EndTooltip();
        }

        // Font Size
        IMGUI_SEPARATOR_TEXT("Font Size", separatorOffset);
        ImGui::DragInt("##font size", &_fontSize, 1.0f);
        _fontSize = std::max(1, _fontSize); // Clamp

        // Font Styles




        if (ImGui::Button("Import"))
        {
            try
            {
                ImportFont();
            }
            catch (const std::exception& exception)
            {
                UmLogger.Log(LogLevel::LEVEL_ERROR, exception.what());
            }
            ImGui::CloseCurrentPopup();
        }
    }
}

void Importer::SpriteFontImporter::ImportFont()
{
    if (nullptr == _makeSpriteFont)
        throw std::exception("MakeSpriteFont function is not loaded.");

    _options.clear();

    // option1. Source
    _options.push_back(_sourcePath.wstring());

    // option2. Output
    const std::wstring outputFileName = _sourcePath.filename().replace_extension(".spritefont").wstring();
    _options.push_back(std::wstring(_importPath.wstring() + L'/' + outputFileName));

    // option3. CharacterRegion
    const std::wstring enRegion = L"/CharacterRegion:0x0-0xff"; // 영어 알파벳과 숫자
    _options.push_back(enRegion);
    const std::wstring koRegion = L"/CharacterRegion:0xAC00-0xD7AF"; // 한글
    _options.push_back(koRegion);

    // option4. DefaultCharacter


    // option4. FontSize
    const std::wstring fontSize = L"/FontSize:" + std::to_wstring(_fontSize);
    _options.push_back(fontSize);

    // option5. FontStyle



    if (int result = _makeSpriteFont(_options); 0 != result)
        throw std::exception("Failed to import sprite font. MakeSpriteFont returned non-zero result.");
}
