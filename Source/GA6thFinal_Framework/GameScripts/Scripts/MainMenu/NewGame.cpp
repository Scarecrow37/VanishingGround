#include "pchScripts.h"
#include "NewGame.h"
#include "SceneTransition/SceneTransitionComponent.h"
#include "PlayerSystem/PlayerSystem.h"
#include "Map/MapManager.h"
#include "QTE/System/QTESystem.h"

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
    SetSubmitAudioID("-101000");
}

NewGame::~NewGame() = default;

void NewGame::Submit()
{
    Base::Submit();
    TransitionToNextScene();
    if (QTESystem* system = SingletonComponent<QTESystem>::GetInstance())
    {
        system->ResetState();
    }
}

void NewGame::Update() 
{
     Debugger()([this] {
        // 아래는 디버그용 코드입니다.
        if (ImGui::Button("Start New Game"))
        {
            TransitionToNextScene();
        }
    });
}

void NewGame::TransitionToNextScene()
{
    File::Path  path              = File::Guid(ReflectFields->NextSceneGuid).ToPath();
    GameObject* transitionManager = SingletonObject<SceneTransitionComponent>::GetInstance();
    if (transitionManager)
    {
        auto transitionComponent = transitionManager->GetComponent<SceneTransitionComponent>();
        if (transitionComponent)
        {
            transitionComponent->SceneTransitionFade("in", "out", [path]() {
                UmSceneManager.LoadScene(path.string());
                if (PlayerSystem* playerSystem = SingletonComponent<PlayerSystem>::GetInstance())
                {
                    playerSystem->SetStatsGameStart();
                }
                if (GameObject* mapManager = SingletonObject<MapManager>::GetInstance())
                {
                    GameObject::Destroy(mapManager);
                }
            });
        }
    }
}
