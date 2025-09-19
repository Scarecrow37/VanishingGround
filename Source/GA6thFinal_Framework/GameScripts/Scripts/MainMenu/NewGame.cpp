#include "pchScripts.h"
#include "NewGame.h"

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
                    ReflectFields->NextScene = data->GetPath().string();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

NewGame::~NewGame() = default;

void NewGame::Submit()
{
    UmSceneManager.LoadScene(ReflectFields->NextScene);
}