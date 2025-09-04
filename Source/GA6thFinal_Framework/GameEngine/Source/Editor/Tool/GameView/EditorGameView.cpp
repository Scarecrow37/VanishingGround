#include "pch.h"
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

void EditorGameView::SetCameraMode(CameraMode mode) 
{
    std::shared_ptr<Camera> camera          = UmGraphics.GetCamera("Game");
    CameraComponent*        cameraComponent = ESceneManager::Engine::GetMainCamera();
    if (camera && cameraComponent && cameraComponent->gameObject->IsValid())
    {
        switch (mode)
        {
            case CAMERA_MODE_DEFAULT: {
                Transform& tr = cameraComponent->gameObject->transform;
                camera->SetWorldMatrix(tr.GetWorldMatrix());
                _cameraMode = CAMERA_MODE_DEFAULT;
                break;
            }
            case CAMERA_MODE_FREE_MANIPULATE: {
                _freeCamera->SetTarget(camera);
                _freeCamera->SetPosition(camera->GetPosition());
                _freeCamera->SetRotation(camera->GetRotation());
                _freeCamera->SetPivot(0.0f);
                _cameraMode = CAMERA_MODE_FREE_MANIPULATE;
                break;
            default:
                break;
            }
        }
    }
}

void EditorGameView::ShowPopupCameraMode() 
{
    if (ImGui::MenuItem("Default Mode", "", _cameraMode == CAMERA_MODE_DEFAULT))
    {
        SetCameraMode(EditorGameView::CAMERA_MODE_DEFAULT);
    }
    ImGuiHelper::HoveredToolTip((const char*)u8"메인 카메라를 기준으로 보여집니다.");
    if (ImGui::MenuItem("Manipulate Mode", "", _cameraMode == CAMERA_MODE_FREE_MANIPULATE))
    {
        SetCameraMode(EditorGameView::CAMERA_MODE_FREE_MANIPULATE);
    }
    ImGuiHelper::HoveredToolTip((const char*)u8"카메라를 직접 조작할 수 있는 모드입니다.");
}

void EditorGameView::OnFramePopupOpened()
{
    ImGui::Separator();
    ImGuiHelper::AlignedText("Camera Mode", ImGuiHelper::CENTER, 0.8f);
    ShowPopupCameraMode();
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
    }
}

void EditorGameView::OnFrameFocusStay()
{
    if (_cameraMode == CAMERA_MODE_FREE_MANIPULATE)
    {
        _freeCamera->Update();
    }
}