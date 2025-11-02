#include "pchScripts.h"
#include "EndingDialogManager.h"
#include "SceneTransition/SceneTransitionComponent.h"
#include "UI/Animations/ChildsAnimationsController/ChildsAnimationsController.h"
UMREAL_COMPONENT(EndingDialogManager)

EndingDialogManager::EndingDialogManager()
{
    MainMenuScene.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".UmScene")
                {
                    ReflectFields->MainMenuSceneGuid = data->GetGuid().string();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}
EndingDialogManager::~EndingDialogManager() = default;

void EndingDialogManager::Reset()
{
    _isDialogEnded = false;
    _isSequencePlaying = false;
    _currentDialogIndex = 0;    
    _currentTimer       = 0.0f;
    _totalDialogCount   = 0;
    _isDialogFading     = false;
    _skipRequested      = false;

    BindInputAction(ControllerButton::A, Action::PRESSED, this, &EndingDialogManager::SkipCurrentDialog);

    auto EndingDialog = GameObject::FindWithTag("EndingDialogPannel").lock();
    if (EndingDialog.get())
    {
        _childsAnimationsController = EndingDialog->GetComponent<ChildsAnimationsController>();
        if (!_childsAnimationsController)
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, "ChildsAnimationsController not found in EndingDialogPannel!");
        }
        else
        {
            _childsAnimationsController->FindAnimations();
        }
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "EndingDialogPannel not found!");
    }
}

void EndingDialogManager::Awake()
{
    StartDialogSequence();
}

void EndingDialogManager::Update()
{
#ifdef _UMEDITOR
    if (ImGui::IsKeyPressed(ImGuiKey_A))
    {
        PressAButton();
    }
#endif
    if (_isSequencePlaying && _childsAnimationsController)
    {
        if (_skipRequested && _currentDialogIndex > 0)
        {
            size_t currentIndex = _currentDialogIndex - 1;
            _childsAnimationsController->CompleteFadeImmediately(currentIndex);
            _isDialogFading = false;
            _currentTimer   = ReflectFields->DialogInterval;
            _skipRequested  = false;
        }

        if (_isDialogFading && _currentDialogIndex > 0)
        {
            size_t currentIndex = _currentDialogIndex - 1;
            if (_childsAnimationsController->IsFadeComplete(currentIndex))
            {
                _isDialogFading = false;
                _currentTimer   = 0.0f;
            }
        }

        _currentTimer += UmTime.DeltaTime();

        if (!_isDialogFading && _currentTimer >= ReflectFields->DialogInterval)
        {
            PlayNextDialog();
            _currentTimer = 0.0f;
        }
    }

    if (_isDialogEnded && _goToMainMenuSceneFlag)
    {
        _isDialogEnded = false;
        _goToMainMenuSceneFlag = false;
        TransitionToMainMenuScene();
    }
}


void EndingDialogManager::TransitionToMainMenuScene()
{
    if (ReflectFields->MainMenuSceneGuid.empty())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "MainMenuSceneGuid is empty!");
        return;
    }
    File::Path path = File::Guid(ReflectFields->MainMenuSceneGuid).ToPath();   
    GameObject* transitionManager = SingletonObject<SceneTransitionComponent>::GetInstance();
    if (transitionManager)
    {
        auto transitionComponent = transitionManager->GetComponent<SceneTransitionComponent>();
        if (transitionComponent)
        {
            transitionComponent->SceneTransitionFade("in_fast", "out_fast",
                                                     [path]() { UmSceneManager.LoadScene(path.string()); });
        }
    }
}

void EndingDialogManager::StartDialogSequence() 
{
    if (!_childsAnimationsController)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "ChildsAnimationsController is null!");
        return;
    }

    _totalDialogCount = _childsAnimationsController->transform->ChildCount;
    if (_totalDialogCount == 0)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "No dialog found in ChildsAnimationsController!");
        return;
    }

    _isSequencePlaying = true;
    _currentDialogIndex = 0;
    _currentTimer       = 0.0f;
    _isDialogEnded      = false;

    PlayNextDialog();
}

void EndingDialogManager::PlayNextDialog()
{
    if (!_childsAnimationsController)
    {
        return;
    }

    if (_currentDialogIndex >= _totalDialogCount)
    {
        _isSequencePlaying = false;
        _isDialogEnded     = true;
        return;
    }
    _childsAnimationsController->FadeIn(_currentDialogIndex);
    _isDialogFading = true;
    _currentDialogIndex++;
}

void EndingDialogManager::SkipCurrentDialog(const Input::Controller& contorller)
{
    PressAButton();
}

void EndingDialogManager::PressAButton() 
{
    if (!_isSequencePlaying)
    {
        if (_isDialogEnded)
        {
            _goToMainMenuSceneFlag = true;
        }
        return;
    }
    _skipRequested = true;
}
