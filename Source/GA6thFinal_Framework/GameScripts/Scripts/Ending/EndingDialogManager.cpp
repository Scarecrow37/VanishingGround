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
    auto EndingDialog = GameObject::FindWithTag("EndingDialogPannel").lock();
    if (EndingDialog.get())
    {
        _childsAnimationsController = EndingDialog->GetComponent<ChildsAnimationsController>();
        if (!_childsAnimationsController)
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, "ChildsAnimationsController not found in EndingDialogPannel!");
        }
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "EndingDialogPannel not found!");
    }
}

void EndingDialogManager::Awake() {}

void EndingDialogManager::Update()
{
    if (_isDialogEnded)
    {
        _isDialogEnded = false;
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
