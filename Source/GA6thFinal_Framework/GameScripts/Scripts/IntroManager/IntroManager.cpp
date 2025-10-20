#include "pchScripts.h"
#include "IntroManager.h"

#include "DifficultyManager/DifficultyManager.h"
#include "ExcelDataSystem/ExcelDataSystem.h"
#include "SceneTransition/SceneTransitionComponent.h"
#include "UI/Animations/FadeDescriptionPanel/FadeDescriptionPanel.h"
#include "UI/Animations/FadeImageElement/FadeImageElement.h"
#include "UI/Animations/FadeTextElement/FadeTextElement.h"
#include "Utility/SingletonHelper.h"

UMREAL_COMPONENT(IntroManager)

IntroManager::IntroManager()
    : _step(Step::WAIT_INTRO_DESCRIPTION), _isLevelSelected(false), _isSelectHard(false), _introDescription(nullptr),
      _normalLevelText(nullptr),
      _hardLevelText(nullptr), _promptText(nullptr), _normalSelection(nullptr), _hardSelection(nullptr)
{
    NextScene.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".UmScene")
                {
                    ReflectFields->NextScene = data->GetGuid().string();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

void IntroManager::Awake()
{
    Component::Awake();

    BindInputAction(ControllerButton::A, Action::PRESSED, this, &IntroManager::SkipStep);
    BindInputAction(ControllerButton::DPAD_UP, Action::PRESSED, this, &IntroManager::SelectNormal);
    BindInputAction(ControllerButton::DPAD_DOWN, Action::PRESSED, this, &IntroManager::SelectHard);
}

void IntroManager::Start()
{
    Component::Start();

    std::unique_ptr<ExcelDataBase> data = nullptr;

    if (const GameObject* excelDataSystem = SingletonObject<ExcelDataSystem>::GetInstance())
    {
        if (ExcelDataSystem* excelDataSystemComponent = excelDataSystem->GetComponent<ExcelDataSystem>())
        {
            const std::u8string sheetName(u8"텍스트");
            if (std::unique_ptr<ExcelDataBase> dataBase = excelDataSystemComponent->FindExcelDataBase(sheetName); nullptr != dataBase)
            {
                data = std::move(dataBase);
            }
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, "Load Text Fail.");
        }
    }


    _introDescription = GetElement<FadeDescriptionPanel>("Intro Description");
    if (nullptr != _introDescription)
    {
        _introDescription->FadeDuration = ReflectFields->FadeDuration;
        if (data)
        {
            if (const size_t rowIndex = data->FindRowIndex(BOOK_SELECT_INTRO_DESC_ID, COLUMN_KEY_ID);
                rowIndex != ExcelDataBase::FIND_INDEX_FAIL)
            {
                const std::string_view description = data->FindData(rowIndex, COLUMN_KEY_CONTENT);
                if (description != ExcelDataBase::FIND_STR_FAIL)
                {
                    _introDescription->Description = description.data();
                }
            }
        }
    }

    _normalLevelText = GetElement<FadeTextElement>("Normal Level Text");
    if (nullptr != _normalLevelText)
    {
        _normalLevelText->FadeDuration = ReflectFields->FadeDuration;
        if (data)
        {
            if (const size_t rowIndex = data->FindRowIndex(BOOK_SELECT_INTRO_NORMAL_ID, COLUMN_KEY_ID);
                rowIndex != ExcelDataBase::FIND_INDEX_FAIL)
            {
                const std::string_view normalText = data->FindData(rowIndex, COLUMN_KEY_CONTENT);
                if (normalText != ExcelDataBase::FIND_STR_FAIL)
                {
                    _normalLevelText->Text = normalText.data();
                }
            }
        }
    }

    _hardLevelText   = GetElement<FadeTextElement>("Hard Level Text");
    if (nullptr != _hardLevelText)
    {
        _hardLevelText->FadeDuration = ReflectFields->FadeDuration;
        if (data)
        {
            if (const size_t rowIndex = data->FindRowIndex(BOOK_SELECT_INTRO_HARD_ID, COLUMN_KEY_ID);
                rowIndex != ExcelDataBase::FIND_INDEX_FAIL)
            {
                const std::string_view hardText = data->FindData(rowIndex, COLUMN_KEY_CONTENT);
                if (hardText != ExcelDataBase::FIND_STR_FAIL)
                {
                    _hardLevelText->Text = hardText.data();
                }
            }
        }
    }

    _promptText      = GetElement<FadeTextElement>("Intro Prompt");
    if (nullptr != _promptText)
    {
        _promptText->FadeDuration = ReflectFields->FadeDuration;
        if (data)
        {
            if (const size_t rowIndex = data->FindRowIndex(BOOK_SELECT_INTRO_BOOK_NAME_PROMPT_ID, COLUMN_KEY_ID);
                rowIndex != ExcelDataBase::FIND_INDEX_FAIL)
            {
                const std::string_view promptText = data->FindData(rowIndex, COLUMN_KEY_CONTENT);
                if (promptText != ExcelDataBase::FIND_STR_FAIL)
                {
                    _promptText->Text = promptText.data();
                }
            }
        }
    }

    _normalSelection = GetElement<FadeImageElement>("Normal Level Selection");
    _hardSelection   = GetElement<FadeImageElement>("Hard Level Selection");
}

void IntroManager::Update()
{
    Component::Update();

    if (_step != Step::WAIT_PROMPT || true == _isLevelSelected)
        _elapsedTime += UmTime.DeltaTime();

    switch (_step)
    {
    case Step::WAIT_INTRO_DESCRIPTION:
        if (_elapsedTime >= GetWaitDescriptionTime())
        {
            if (nullptr != _introDescription)
            {
                _introDescription->FadeIn();
            }
            _step = Step::FADE_IN_INTRO_DESCRIPTION;
        }
        break;
    case Step::FADE_IN_INTRO_DESCRIPTION:
        if (_elapsedTime >= GetFadeDescriptionTime())
        {
            _step = Step::WAIT_LEVEL_SELECTION;
        }
        break;
    case Step::WAIT_LEVEL_SELECTION:
        if (_elapsedTime >= GetWaitLevelSelectionTime())
        {
            if (nullptr != _normalLevelText)
            {
                _normalLevelText->FadeIn();
            }
            if (nullptr != _hardLevelText)
            {
                _hardLevelText->FadeIn();
            }
            _step = Step::FADE_IN_LEVEL_SELECTION;
        }
        break;
    case Step::FADE_IN_LEVEL_SELECTION:
        if (_elapsedTime >= GetFadeLevelSelectionTime())
        {
            _step = Step::WAIT_PROMPT;
            SelectNormal();
        }
        break;
    case Step::WAIT_PROMPT:
        if (_elapsedTime >= GetWaitPromptTime())
        {
            if (nullptr != _promptText)
            {
                _promptText->FadeIn();
            }
            _step = Step::FADE_IN_PROMPT;
        }
        break;
    case Step::FADE_IN_PROMPT:
        if (_elapsedTime >= GetFadePromptTime())
        {
            _step = Step::END;
        }
        break;
    case Step::END:
        DifficultyManager* difficultyManager = SingletonComponent<DifficultyManager>::GetInstance();
        if (difficultyManager)
        {
            difficultyManager->SetDifficulty(_isSelectHard ? DifficultyManager::Difficulty::HARD
                                                           : DifficultyManager::Difficulty::NORMAL);
        }
        LoadNextScene();
        break;
    }
}

void IntroManager::Reset()
{
    Component::Reset();

    _step             = Step::WAIT_INTRO_DESCRIPTION;
    _introDescription = nullptr;
    _normalLevelText  = nullptr;
    _hardLevelText    = nullptr;
    _promptText       = nullptr;
    _elapsedTime      = 0.0f;
    _isLevelSelected  = false;
    _isSelectHard     = false;
}

float IntroManager::GetWaitDescriptionTime() const
{
    return ReflectFields->DescriptionDelay;
}

float IntroManager::GetFadeDescriptionTime() const
{
    return GetWaitDescriptionTime() + ReflectFields->FadeDuration;
}

float IntroManager::GetWaitLevelSelectionTime() const
{
    return GetFadeDescriptionTime() + ReflectFields->LevelSelectDelay;
}

float IntroManager::GetFadeLevelSelectionTime() const
{
    return GetWaitLevelSelectionTime() + ReflectFields->FadeDuration;
}

float IntroManager::GetWaitPromptTime() const
{
    return GetFadeLevelSelectionTime() + ReflectFields->PromptDelay;
}

float IntroManager::GetFadePromptTime() const
{
    return GetWaitPromptTime() + ReflectFields->FadeDuration;
}

void IntroManager::LoadNextScene() const
{
    File::Path path = File::Guid(ReflectFields->NextScene).ToPath();

    if (const GameObject* transitionManager = SingletonObject<SceneTransitionComponent>::GetInstance())
    {
        if (const auto transitionComponent = transitionManager->GetComponent<SceneTransitionComponent>())
        {
            transitionComponent->SceneTransitionFade("in", "out",
                                                     [path]() { UmSceneManager.LoadScene(path.string()); });
        }
        else
        {
            UmSceneManager.LoadScene(path.string());
        }
    }
}

void IntroManager::SkipStep(const Input::Controller& controller)
{
    switch (_step)
    {
    case Step::WAIT_INTRO_DESCRIPTION:
        _elapsedTime = GetWaitDescriptionTime();
        if (nullptr != _introDescription)
        {
            _introDescription->FadeIn();
        }
        _step = Step::FADE_IN_INTRO_DESCRIPTION;
        break;
    case Step::FADE_IN_INTRO_DESCRIPTION:
        _elapsedTime = GetFadeDescriptionTime();
        if (nullptr != _introDescription)
        {
            _introDescription->End();
        }
        _step = Step::WAIT_LEVEL_SELECTION;
        break;
    case Step::WAIT_LEVEL_SELECTION:
        _elapsedTime = GetWaitLevelSelectionTime();
        if (nullptr != _normalLevelText)
        {
            _normalLevelText->FadeIn();
        }
        if (nullptr != _hardLevelText)
        {
            _hardLevelText->FadeIn();
        }
        _step = Step::FADE_IN_LEVEL_SELECTION;
        break;
    case Step::FADE_IN_LEVEL_SELECTION:
        _elapsedTime = GetFadeLevelSelectionTime();
        if (nullptr != _normalLevelText)
        {
            _normalLevelText->End();
        }
        if (nullptr != _hardLevelText)
        {
            _hardLevelText->End();
        }
        _step = Step::WAIT_PROMPT;
        break;
    case Step::WAIT_PROMPT:
        _isLevelSelected = true;
        break;
    case Step::FADE_IN_PROMPT:
        _step = Step::END;
        break;
    default:
        break;
    }
}

void IntroManager::SelectNormal(const Input::Controller& controller)
{
    SelectNormal();
}

void IntroManager::SelectNormal()
{
    if (_step == Step::WAIT_PROMPT)
    {
        if (_hardSelection)
            _hardSelection->FadeOut();
        if (_normalSelection)
            _normalSelection->FadeIn();
        _isSelectHard = false;
    }
}

void IntroManager::SelectHard(const Input::Controller& controller)
{
    if (_step == Step::WAIT_PROMPT)
    {
        if (_normalSelection)
            _normalSelection->FadeOut();
        if (_hardSelection)
            _hardSelection->FadeIn();
        _isSelectHard = true;
    }
}