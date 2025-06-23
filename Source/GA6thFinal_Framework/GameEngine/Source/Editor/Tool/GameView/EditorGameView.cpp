#include "pch.h"
#include "UmScripts.h"

EditorGameView::EditorGameView() 
{
    SetLabel("Game");
    SetDockLayout(ImGuiDir_Up);
}

EditorGameView::~EditorGameView() {}

void EditorGameView::OnFrameRender() 
{
    CameraComponent* camera = ESceneManager::Engine::GetMainCamera();
    if (camera && camera->gameObject->IsValid())    
    {
        auto gpuHandle = UmRenderer.GetRenderSceneImage("Game");
        ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
        ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
        ImVec2 size;
        size.x = contentMax.x - contentMin.x;
        size.y = size.x / (float)camera->Aspect;
        ImGui::Image((ImTextureID)gpuHandle.ptr, size);
    }
}
