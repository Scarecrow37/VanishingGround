#include "pchScripts.h"
#include "Credits.h"
#include "SceneTransition/SceneTransitionComponent.h"

UMREAL_COMPONENT(Credits)

Credits::Credits()
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
Credits::~Credits() = default;

void Credits::Submit()
{
    TransitionToCreditsScene();
}

void Credits::TransitionToCreditsScene()
{
    if (ReflectFields->NextSceneGuid.empty())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "NextSceneGuid is empty!");
        return;
    }
    File::Path path = File::Guid(ReflectFields->NextSceneGuid).ToPath();   
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
