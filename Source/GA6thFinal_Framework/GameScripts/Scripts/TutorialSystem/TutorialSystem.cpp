#include "pchScripts.h"
#include "TutorialSystem.h"

#include "ExcelDataSystem/ExcelDataSystem.h"
#include "UI/Elements/Text/TextElement.h"
#include "UI/Panels/Description/DescriptionPanel.h"
#include "UI/Elements/Image/ImageElement.h"

UMREAL_COMPONENT(TutorialSystem)

TutorialSystem::TutorialSystem() = default;

void TutorialSystem::Awake()
{
    Component::Awake();

    if (_singletonObject.TrySingleTon(true))
    {
        _singletonComponent.TrySingleTon();
    }
}

void TutorialSystem::Start()
{
    Component::Start();

    FindComponents();
    Hide();

    SetupData();
}

void TutorialSystem::ImGuiDrawPropertysEvent()
{
    Component::ImGuiDrawPropertysEvent();

    if (ImGui::TreeNodeEx("Tutorial Data##details"))
    {
        if (ImGui::BeginTable("TutorialTable##Details", 3, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg))
        {
            // Headers
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthStretch, 0.3f);
            ImGui::TableSetupColumn("Title", ImGuiTableColumnFlags_WidthStretch, 0.4f);
            ImGui::TableSetupColumn("Image AssetID", ImGuiTableColumnFlags_WidthStretch, 0.3f);
            ImGui::TableHeadersRow();

            for (const auto& [id, tutorial] : _tutorials)
            {
                ImGui::PushID(id);
                ImGui::TableNextRow();
                // ID
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", id);
                // Title
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(tutorial.Title.c_str());
                // Image AssetID
                ImGui::TableSetColumnIndex(2);
                const int assetID = UmFileSystem.GetAssetIDFromGuid(tutorial.Image);
                ImGui::Text("%d", assetID);
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }
}

void TutorialSystem::FindComponents()
{
    _panel       = GameObject::FindWithTag(OBJECT_TAG_PANEL);
    _title       = GameObject::FindComponentWithTag<TextElement>(OBJECT_TAG_TITLE);
    _description = GameObject::FindComponentWithTag<DescriptionPanel>(OBJECT_TAG_DESCRIPTION);
    _image       = GameObject::FindComponentWithTag<ImageElement>(OBJECT_TAG_IMAGE);
}

void TutorialSystem::Hide() const
{
    if (const auto panel = _panel.lock(); nullptr != panel)
    {
        panel->SetActive(false);
    }
}

void TutorialSystem::SetupData()
{
    if (const GameObject* excelDataSystem = SingletonObject<ExcelDataSystem>::GetInstance())
    {
        if (ExcelDataSystem* excelDataSystemComponent = excelDataSystem->GetComponent<ExcelDataSystem>())
        {
            if (const std::unique_ptr<ExcelDataBase> dataBase = excelDataSystemComponent->FindExcelDataBase(SHEET_NAME); nullptr != dataBase)
            {
                const size_t rowCount = dataBase->RowCount();
                for (size_t row = 1; row < rowCount; ++row)
                {
                    std::string_view idStringView = dataBase->FindData(row, COLUMN_KEY_ID);
                    std::string      idString     = std::string(idStringView);
                    int              id           = std::stoi(idString);

                    std::string_view  titleStringView = dataBase->FindData(row, COLUMN_KEY_TITLE);
                    const std::string titleString     = std::string(titleStringView);

                    std::string_view  descriptionStringView = dataBase->FindData(row, COLUMN_KEY_DESCRIPTION);
                    const std::string descriptionString     = std::string(descriptionStringView);

                    std::string_view imageStringView = dataBase->FindData(row, COLUMN_KEY_IMAGE);
                    std::string      imageString     = std::string(imageStringView);
                    const int        image           = std::stoi(idString);
                    File::Guid       imageGuid       = UmFileSystem.GetGuidFromAssetID(image);

                    auto [_, succeed] = _tutorials.try_emplace(id, false, titleString, descriptionString, imageGuid);
                    assert(succeed);
                }
            }
            else
            {
                UmLogger.Log(LogLevel::LEVEL_WARNING, "Can not find tutorial sheet.");
            }
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, "Load Tutorial Data Fail.");
        }
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Excel Data System is not existed.");
    }
}