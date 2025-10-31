#include "pchScripts.h"
#include "CreditsBackButton.h"
#include "SceneTransition/SceneTransitionComponent.h"

UMREAL_COMPONENT(CreditsBackButton)

CreditsBackButton::CreditsBackButton()
{
    NextScene.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".UmScene")
                {
                    ReflectFields->NextSceneGuid = data->GetGuid().string();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}
CreditsBackButton::~CreditsBackButton() = default;

void CreditsBackButton::Submit()
{
    TransitionToMainMenuScene();
}

void CreditsBackButton::FocusIn(FocusCallType callType) 
{
    Base::FocusIn(callType);
}

void CreditsBackButton::TransitionToMainMenuScene()
{
    if (ReflectFields->NextSceneGuid.empty())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "NextSceneGuid is empty!");
        return;
    }
    File::Path  path              = File::Guid(ReflectFields->NextSceneGuid).ToPath();
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
