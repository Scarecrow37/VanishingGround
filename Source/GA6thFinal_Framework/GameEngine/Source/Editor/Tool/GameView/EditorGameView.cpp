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
        auto   gpuHandle  = UmGraphics.GetRenderSceneImage("Game");
        ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
        ImVec2 contentMax = ImGui::GetWindowContentRegionMax();

        ImVec2 fullSize;
        fullSize.x = contentMax.x - contentMin.x;
        fullSize.y = contentMax.y - contentMin.y;

        ImVec2 size;
        float aspect = camera->Aspect;
        if (fullSize.x / fullSize.y > aspect)
        {
            // 세로가 제한 요소 → 세로 기준 최대 크기, 가로는 비율로 맞춤
            size.y = std::round(fullSize.y);
            size.x = std::round(size.y * aspect);
        }
        else
        {
            // 가로가 제한 요소 → 가로 기준 최대 크기, 세로는 비율로 맞춤
            size.x = std::round(fullSize.x);
            size.y = std::round(size.x / aspect);
        }

        ImVec2 centerOffset = ImVec2((fullSize.x - size.x) * 0.5f, (fullSize.y - size.y) * 0.5f);
        ImVec2 startPos = ImGui::GetWindowPos() + contentMin + centerOffset;
        ImGui::SetCursorScreenPos(startPos);
        ImGui::Image((ImTextureID)gpuHandle.ptr, size);
    }
}
