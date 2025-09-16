#include "pch.h"
#include "GizmoManager.h"
#include "Editor/Tool/Scene/Command/EditorSceneCommands.h"

EGizmoManager::EGizmoManager() 
{
    _targetWindow = nullptr;
    _targetCamera = nullptr;
}

void EGizmoManager::SubmitSceneGizmoIcon(SceneGizmo* gizmo)
{
    //활성화된 기즈모만 그림.
    if (gizmo->_ownerComponenet.gameObject->IsValid())
    {
        _sceneGizmosIcon.emplace_back(gizmo->_ownerComponenet.GetWeakPtr(), gizmo);
    }  
}

void EGizmoManager::SubminSceneImGuizmo(SceneGizmo* gizmo) 
{
    if (gizmo->_ownerComponenet.gameObject->IsValid())
    {
        if (nullptr != gizmo->_ownerMatrix)
        {
            _sceneImGuizmos.emplace_back(gizmo->_ownerComponenet.GetWeakPtr(), gizmo);
        }
    }
}

void EGizmoManager::BeginDraw(ImGuiWindow* targetWindow, Camera* camera) 
{
    _targetWindow = targetWindow;
    _targetCamera = camera;
}

void EGizmoManager::Draw()
{
    if (_targetWindow && _targetCamera)
    {
        // 유효한 기즈모만 그린다.
        std::erase_if(_sceneGizmosIcon, 
        [this](const std::pair<std::weak_ptr<Component>, SceneGizmo*>& pair) 
        {
            // 생명 여부 확인
            const std::weak_ptr<Component>&  weakOwner = pair.first;
            const std::shared_ptr<Component> owner     = weakOwner.lock();
            if (nullptr == owner)
            {
                return true;
            }
               
            // 프러스텀 컬링
            BoundingFrustum frustum;
            BoundingFrustum::CreateFromMatrix(frustum, _targetCamera->GetProjectionMatrix());
            frustum.Transform(frustum, _targetCamera->GetWorldMatrix());

            BoundingBox box;
            box.Extents = {2, 2, 2};
            SceneGizmo&   gizmo       = *pair.second;
            const Matrix& worldMatrix =  gizmo._ownerMatrix != nullptr ? *gizmo._ownerMatrix : gizmo._ownerComponenet.transform->GetWorldMatrix();
            box.Transform(box, owner->transform->GetWorldMatrix());
            bool intersect = false == frustum.Intersects(box);
            return intersect;
        });

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); 
        for (auto& [weakOwner, gizmo] : _sceneGizmosIcon)
        {
            if (gizmo->_icon)
            {
                ImGui::PushID(gizmo);
                {
                    if (ImVec2 screenPos; true == CalculateGizmoScreenPosition(*gizmo, &screenPos))
                    {
                        const ImGuiViewport*               viewport    = ImGui::GetMainViewport();
                        const D3D12_GPU_DESCRIPTOR_HANDLE& imageHandle = gizmo->_icon->GetGPUHandle();

                        // 이미지 크기에 맞게 중심 이동
                        screenPos.x -= gizmo->Size.x * 0.5f;
                        screenPos.y -= gizmo->Size.y * 0.5f;
                        ImGui::SetCursorPos(screenPos);
                        if (ImGui::ImageButton((ImTextureID)imageHandle.ptr, gizmo->Size))
                        {
                            if (gizmo->EventListener == nullptr)
                            {
                                std::weak_ptr<GameObject> oldWp = EditorHierarchyTool::GetFocusObject();
                                UmCommandManager.Do<Command::Hierarchy::FocusCommand>(oldWp, gizmo->_ownerComponenet.gameObject->GetWeakPtr());
                            }
                            else
                            {
                                gizmo->EventListener.Invoke();
                            }
                        }
                    }
                }
                ImGui::PopID();
            }
        }
        ImGui::PopStyleColor(3);
    } 
    else
    {
        assert(!"BeginDraw를 먼저 호출해야 합니다.");
    }
}

void EGizmoManager::EndDraw() 
{
    _targetWindow = nullptr;
    _targetCamera = nullptr;
    _sceneGizmosIcon.clear();
    _sceneImGuizmos.clear();
}

void EGizmoManager::DrawImGuizmo(ImGuiHelper::DrawManipulateDesc& desc)
{
    if (_targetCamera)
    {
        // 유효한 기즈모만 그린다.
        std::erase_if(_sceneImGuizmos, [this](const std::pair<std::weak_ptr<Component>, SceneGizmo*>& pair) 
        {
            // 생명 여부 확인
            const std::weak_ptr<Component>&  weakOwner = pair.first;
            return weakOwner.expired();
        });

        for (auto& [weakOwner, gizmo] : _sceneImGuizmos)
        {
            ImGuiHelper::DrawManipulate(_targetCamera, gizmo->_ownerMatrix, desc);
        }   
    }
    else
    {
        assert(!"BeginDraw를 먼저 호출해야 합니다.");
    }
}

bool EGizmoManager::CalculateGizmoScreenPosition(SceneGizmo& gizmo, ImVec2* outScreenPos)
{
    if (nullptr == outScreenPos)
    {
        return false;
    }

    //행렬 로드
    const Matrix& targetMatrix = gizmo._ownerMatrix != nullptr ? *gizmo._ownerMatrix : gizmo._ownerComponenet.transform->GetWorldMatrix();
    const DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4(&targetMatrix);
    const DirectX::XMMATRIX viewMatrix  = DirectX::XMLoadFloat4x4(&_targetCamera->GetViewMatrix());
    const DirectX::XMMATRIX projMatrix  = DirectX::XMLoadFloat4x4(&_targetCamera->GetProjectionMatrix());

    // 월드 좌표
    DirectX::XMVECTOR worldPos = worldMatrix.r[3];

    // WVP 변환
    DirectX::XMMATRIX viewProjectionMatrix = DirectX::XMMatrixMultiply(viewMatrix, projMatrix);
    DirectX::XMVECTOR clipPos              = DirectX::XMVector4Transform(worldPos, viewProjectionMatrix);

    // 클립핑
    DirectX::XMFLOAT4 clipPosF;
    DirectX::XMStoreFloat4(&clipPosF, clipPos);  

    // 카메라 뒤에 있으면 실패
    if (clipPosF.w < 0.0f)
    {
        return false;
    }

    // 원근 나눗셈
    DirectX::XMFLOAT3 ndcPos = {clipPosF.x / clipPosF.w, clipPosF.y / clipPosF.w, clipPosF.z / clipPosF.w};

    // 화면 좌표 변환
    const ImVec2& viewportPos  = _targetWindow->ContentRegionRect.Min;
    const ImVec2  viewportSize = _targetWindow->ContentRegionRect.GetSize();
    
    ImVec2 screenPos;
    screenPos.x = (ndcPos.x + 1.0f) * 0.5f * viewportSize.x + viewportPos.x;
    screenPos.y = (1.0f - ndcPos.y) * 0.5f * viewportSize.y + viewportPos.y;

    // ImGui 윈도우 로컬 좌표로 변환
    ImVec2& windowPos = _targetWindow->Pos;
    outScreenPos->x   = screenPos.x - windowPos.x;
    outScreenPos->y   = screenPos.y - windowPos.y;

    return true;
}
