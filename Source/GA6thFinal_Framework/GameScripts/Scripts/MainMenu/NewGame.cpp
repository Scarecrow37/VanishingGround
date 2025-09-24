#include "pchScripts.h"
#include "NewGame.h"
#include "SceneTransition/TransitionManager.h"

UMREAL_COMPONENT(NewGame)

NewGame::NewGame()
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

NewGame::~NewGame() = default;

void NewGame::Submit()
{
    File::Path  path              = File::Guid(ReflectFields->NextSceneGuid).ToPath();
    GameObject* transitionmanager = SingletonObject<TransitionManager>::GetInstance();
    transitionmanager->GetComponent<TransitionManager>()->SceneTransitionFade(
        "in", "out", [path]() { UmSceneManager.LoadScene(path.string()); });
}