#include "pch.h"
#include "UmScripts.h"
#include "Editor/DynamicCamera/EditorDynamicCamera.h"

EditorGameView::EditorGameView() 
    : _cameraMode(CAMERA_MODE_DEFAULT)
    , _freeCamera(new EditorDynamicCamera)
{
    SetLabel("Game");
    SetDockLayout(ImGuiDir_Up);
}

EditorGameView::~EditorGameView()
{
    if (_freeCamera)
    {
        delete _freeCamera;
        _freeCamera = nullptr;
    }
}

void EditorGameView::OnFrameRender() 
{
    std::shared_ptr<Camera> camera = UmGraphics.GetCamera("Game");
    CameraComponent* cameraComponent = ESceneManager::Engine::GetMainCamera();
    if (camera && cameraComponent && cameraComponent->gameObject->IsValid())    
    {
        auto   gpuHandle  = UmGraphics.GetRenderSceneImage("Game");
        ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
        ImVec2 contentMax = ImGui::GetWindowContentRegionMax();

        ImVec2 fullSize;
        fullSize.x = contentMax.x - contentMin.x;
        fullSize.y = contentMax.y - contentMin.y;

        ImVec2 size;
        float  aspect = cameraComponent->Aspect;
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

        if (false == _freeCamera->IsMoved() &&
            false == _freeCamera->IsRotated())
        {
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Default", "", _cameraMode == CAMERA_MODE_DEFAULT))
                {
                    Transform& tr = cameraComponent->gameObject->transform;
                    camera->SetWorldMatrix(tr.GetWorldMatrix());
                    _cameraMode = CAMERA_MODE_DEFAULT;
                }
                if (ImGui::MenuItem("Free Manipulate", "", _cameraMode == CAMERA_MODE_FREE_MANIPULATE))
                {
                    _freeCamera->SetPosition(Vector3::Zero);
                    _freeCamera->SetRotation(Quaternion::Identity);
                    _freeCamera->SetPivot(0.0f);
                    _freeCamera->SetTarget(camera);
                    _cameraMode = CAMERA_MODE_FREE_MANIPULATE;
                }
            }
        }
    }
}

void EditorGameView::OnFrameFocusStay()
{
    if (_cameraMode == CAMERA_MODE_FREE_MANIPULATE)
    {
        _freeCamera->Update();
    }
}