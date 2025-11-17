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
    : _step(Step::WAIT_INTRO_DESCRIPTION), _isLevelSelected(false), _isSelectHard(false)
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
    BindInputAction(ControllerButton::LEFT_THUMB_STICK, Action::PRESSED, this, &IntroManager::SelectThumbStick);

    const std::unique_ptr<ExcelDataBase> data = SetupData();
    FindComponents();
    SetupComponent(data);
    ResetFade();
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
            if (const auto sharedIntroDescription = _introDescription.lock())
            {
                sharedIntroDescription->FadeIn();
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
            if (const auto sharedNormalLevelText = _normalLevelText.lock())
            {
                sharedNormalLevelText->FadeIn();
            }
            if (const auto sharedHardLevelText = _hardLevelText.lock())
            {
                sharedHardLevelText->FadeIn();
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
            if (const auto sharedPromptText = _promptText.lock())
            {
                sharedPromptText->FadeIn();
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
            difficultyManager->SetDifficulty(_isSelectHard ? Difficulty::HARD
                                                           : Difficulty::NORMAL);
        }
        LoadNextScene();
        break;
    }

    Debugger()([this] {
        // 아래는 디버그용 코드입니다.
        if (ImGui::IsKeyPressed(ImGuiKey_UpArrow, false))
        {
            SelectNormal();
        }
        else if (ImGui::IsKeyPressed(ImGuiKey_DownArrow, false))
        {
            SelectHard();
        }
        else if (ImGui::IsKeyPressed(ImGuiKey_A, false))
        {
            SkipStep();
        }
    });
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

void IntroManager::FindComponents()
{
    _introDescription = GameObject::FindComponentWithTag<FadeDescriptionPanel>(TAG_INTRO_DESCRIPTION.data());
    _normalLevelText  = GameObject::FindComponentWithTag<FadeTextElement>(TAG_NORMAL_LEVEL_TEXT.data());
    _hardLevelText    = GameObject::FindComponentWithTag<FadeTextElement>(TAG_HARD_LEVEL_TEXT.data());
    _promptText       = GameObject::FindComponentWithTag<FadeTextElement>(TAG_PROMPT_TEXT.data());
    _normalSelection  = GameObject::FindComponentWithTag<FadeImageElement>(TAG_NORMAL_LEVEL_SELECTION.data());
    _hardSelection    = GameObject::FindComponentWithTag<FadeImageElement>(TAG_HARD_LEVEL_SELECTION.data());
}

std::unique_ptr<ExcelDataBase> IntroManager::SetupData()
{
    if (ExcelDataSystem* excelDataSystemComponent = SingletonComponent<ExcelDataSystem>::GetInstance())
    {
        if (std::unique_ptr<ExcelDataBase> dataBase = excelDataSystemComponent->FindExcelDataBase(SHEET_NAME.data()))
        {
            return std::move(dataBase);
        }
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Load Text Fail.");
    }

    return nullptr;
}

void IntroManager::SetupComponent(const std::unique_ptr<ExcelDataBase>& data)
{
    if (data == nullptr)
        return;

    if (const auto sharedIntroDescription = _introDescription.lock())
    {
        sharedIntroDescription->FadeDuration = ReflectFields->FadeDuration;

        if (const size_t rowIndex = data->FindRowIndex(BOOK_SELECT_INTRO_DESC_ID, COLUMN_KEY_ID);
            rowIndex != ExcelDataBase::FIND_INDEX_FAIL)
        {
            const std::string_view description = data->FindData(rowIndex, COLUMN_KEY_CONTENT);
            if (description != ExcelDataBase::FIND_STR_FAIL)
            {
                sharedIntroDescription->Description = description.data();
            }
        }
    }

    if (const auto sharedNormalLevelText = _normalLevelText.lock())
    {
        sharedNormalLevelText->FadeDuration = ReflectFields->FadeDuration;

        if (const size_t rowIndex = data->FindRowIndex(BOOK_SELECT_INTRO_NORMAL_ID, COLUMN_KEY_ID);
            rowIndex != ExcelDataBase::FIND_INDEX_FAIL)
        {
            const std::string_view normalText = data->FindData(rowIndex, COLUMN_KEY_CONTENT);
            if (normalText != ExcelDataBase::FIND_STR_FAIL)
            {
                sharedNormalLevelText->Text = normalText.data();
            }
        }
    }

    if (const auto sharedHardLevelText = _hardLevelText.lock())
    {
        sharedHardLevelText->FadeDuration = ReflectFields->FadeDuration;

        if (const size_t rowIndex = data->FindRowIndex(BOOK_SELECT_INTRO_HARD_ID, COLUMN_KEY_ID);
            rowIndex != ExcelDataBase::FIND_INDEX_FAIL)
        {
            const std::string_view hardText = data->FindData(rowIndex, COLUMN_KEY_CONTENT);
            if (hardText != ExcelDataBase::FIND_STR_FAIL)
            {
                sharedHardLevelText->Text = hardText.data();
            }
        }
    }

    if (const auto sharedPromptText = _promptText.lock())
    {
        sharedPromptText->FadeDuration = ReflectFields->FadeDuration;
        if (const size_t rowIndex = data->FindRowIndex(BOOK_SELECT_INTRO_BOOK_NAME_PROMPT_ID, COLUMN_KEY_ID);
            rowIndex != ExcelDataBase::FIND_INDEX_FAIL)
        {
            const std::string_view promptText = data->FindData(rowIndex, COLUMN_KEY_CONTENT);
            if (promptText != ExcelDataBase::FIND_STR_FAIL)
            {
                sharedPromptText->Text = promptText.data();
            }
        }
    }
}

void IntroManager::ResetFade()
{
    if (const auto introDescription = _introDescription.lock())
    {
        introDescription->Begin();
    }
    if (const auto normalLevelText = _normalLevelText.lock())
    {
        normalLevelText->Begin();
    }
    if (const auto hardLevelText = _hardLevelText.lock())
    {
        hardLevelText->Begin();
    }
    if (const auto promptText = _promptText.lock())
    {
        promptText->Begin();
    }
    if (const auto normalSelection = _normalSelection.lock())
    {
        normalSelection->Begin();
    }
    if (const auto hardSelection = _hardSelection.lock())
    {
        hardSelection->Begin();
    }

    _step            = Step::WAIT_INTRO_DESCRIPTION;
    _elapsedTime     = 0.0f;
    _isLevelSelected = false;
    _isSelectHard    = false;
}

void IntroManager::SkipStep(const Input::Controller& controller)
{
    SkipStep();
}

void IntroManager::SkipStep()
{
    switch (_step)
    {
    case Step::WAIT_INTRO_DESCRIPTION:
        _elapsedTime = GetWaitDescriptionTime();
        if (const auto sharedIntroDescription = _introDescription.lock())
        {
            sharedIntroDescription->FadeIn();
        }
        _step = Step::FADE_IN_INTRO_DESCRIPTION;
        break;
    case Step::FADE_IN_INTRO_DESCRIPTION:
        _elapsedTime = GetFadeDescriptionTime();
        if (const auto sharedIntroDescription = _introDescription.lock())
        {
            sharedIntroDescription->End();
        }
        _step = Step::WAIT_LEVEL_SELECTION;
        break;
    case Step::WAIT_LEVEL_SELECTION:
        _elapsedTime = GetWaitLevelSelectionTime();
        if (const auto sharedNormalLevelText = _normalLevelText.lock())
        {
            sharedNormalLevelText->FadeIn();
        }
        if (const auto sharedHardLevelText = _hardLevelText.lock())
        {
            sharedHardLevelText->FadeIn();
        }
        _step = Step::FADE_IN_LEVEL_SELECTION;
        break;
    case Step::FADE_IN_LEVEL_SELECTION:
        _elapsedTime = GetFadeLevelSelectionTime();
        if (const auto sharedNormalLevelText = _normalLevelText.lock())
        {
            sharedNormalLevelText->End();
        }
        if (const auto sharedHardLevelText = _hardLevelText.lock())
        {
            sharedHardLevelText->End();
        }
        _step = Step::WAIT_PROMPT;
        SelectNormal();
        break;
    case Step::WAIT_PROMPT:
        _isLevelSelected = true;
        UmAudio.Play("-901002");
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
        if (const auto sharedHardSelection = _hardSelection.lock())
        {
            sharedHardSelection->FadeOut();
        }
        if (const auto sharedNormalSelection = _normalSelection.lock())
        {
            sharedNormalSelection->FadeIn();
        }
        _isSelectHard = false;
        UmAudio.Play("-901000");
    }
}

void IntroManager::SelectHard(const Input::Controller& controller)
{
    SelectHard();
}

void IntroManager::SelectHard() 
{
    if (_step == Step::WAIT_PROMPT)
    {
        if (const auto sharedNormalSelection = _normalSelection.lock())
        {
            sharedNormalSelection->FadeOut();
        }
        if (const auto sharedHardSelection = _hardSelection.lock())
        {
            sharedHardSelection->FadeIn();
        }
        _isSelectHard = true;
        UmAudio.Play("-901000");
    }
}

void IntroManager::SelectThumbStick(const Input::Controller& controller)
{
    Input::Controller::ThumbStickAxis axis = controller.GetLeftThumbStickAxis();
    if (Mathf::Epsilon < axis.Y)
    {
        SelectNormal(controller);
    }
    else if (-Mathf::Epsilon > axis.Y)
    {
        SelectHard(controller);
    }
}