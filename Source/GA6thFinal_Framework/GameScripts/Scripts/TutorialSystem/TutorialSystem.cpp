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
    std::unique_ptr<ExcelDataBase> data = nullptr;

    if (const GameObject* excelDataSystem = SingletonObject<ExcelDataSystem>::GetInstance())
    {
        if (ExcelDataSystem* excelDataSystemComponent = excelDataSystem->GetComponent<ExcelDataSystem>())
        {
            if (std::unique_ptr<ExcelDataBase> dataBase = excelDataSystemComponent->FindExcelDataBase(SHEET_NAME); nullptr != dataBase)
            {
                data = std::move(dataBase);
            }
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, "Load Tutorial Data Fail.");
        }
    }
}