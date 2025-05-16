#include "pch.h"
#include "EditorSceneTool.h"
#include "../..//DynamicCamera/EditorDynamicCamera.h"

EditorSceneTool::EditorSceneTool() 
    :   _camera(std::make_unique<EditorDynamicCamera>())
{
    if (pSceneTool == nullptr)
    {
        pSceneTool = this;
    }
    else
    {
        __debugbreak(); //??? 에디터 2개 생성됨
    }

    SetLabel("Scene");
    SetDockLayout(ImGuiDir_Up);

    drawManipulateDesc.Operation = ImGuizmo::TRANSLATE;
    drawManipulateDesc.Mode      = ImGuizmo::MODE::WORLD;
    drawManipulateDesc.UseSnap = false;
    drawManipulateDesc.Snap = {1.f, 1.f, 1.f};

    drawManipulateDesc.ViewDesc.Size = ImVec2(128, 128);
    drawManipulateDesc.ViewDesc.BackgroundColor = 0x10101010;
}

EditorSceneTool::~EditorSceneTool()
{

}

void EditorSceneTool::SetManipulateObject(std::weak_ptr<GameObject>& object) 
{
    pSceneTool-> _manipulateObject = object;
}

void EditorSceneTool::OnStartGui()
{
    std::shared_ptr<Camera> camera = UmRenderer.GetCamera("Editor");
    ASSERT((nullptr != camera), L"Camera is nullptr");

    _camera->SetTarget(camera);
}

void EditorSceneTool::OnPreFrameBegin()
{
    SetMoveFlag();
}

void EditorSceneTool::OnPostFrameBegin()
{
}

void EditorSceneTool::OnFrameRender() 
{
    UpdateMode();
    _camera->Update();

    SetCamera();
    DrawSceneView();
    DrawManipulate();
}

void EditorSceneTool::OnFrameEnd()
{
}

void EditorSceneTool::OnFrameFocusStay()
{
}
    
void EditorSceneTool::SetMoveFlag() 
{
    if (true == _isOver)
    {
        SetImGuiWindowFlag(ImGuiWindowFlags_NoMove);
    }
    else
    {
        SetImGuiWindowFlag(ImGuiWindowFlags_None);
    }
}

void EditorSceneTool::SetCamera()
{
    ImVec2 windowPos  = ImGui::GetWindowPos();
    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();

    _clientLeft   = windowPos.x + contentMin.x;
    _clientRight  = windowPos.x + contentMax.x;
    _clientTop    = windowPos.y + contentMin.y;
    _clientBottom = windowPos.y + contentMax.y;

    _clientWidth  = _clientRight - _clientLeft;
    _clientHeight = _clientBottom - _clientTop;
    ReflectFields->CameraAspect = _clientWidth / _clientHeight;

    auto& camera = _camera->GetCamera();
    camera->SetupPerspective(
        ReflectFields->CameraFov,
        ReflectFields->CameraAspect,
        ReflectFields->CameraNearZ,
        ReflectFields->CameraFarZ);   
}

void EditorSceneTool::UpdateMode()
{
    if (false == ImGui::IsKeyDown(ImGuiKey_MouseRight))
    {
        if (ImGui::IsKeyPressed(ImGuiKey_W))
            drawManipulateDesc.Operation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_E))
            drawManipulateDesc.Operation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R))
            drawManipulateDesc.Operation = ImGuizmo::SCALE;
        if (ImGui::IsKeyPressed(ImGuiKey_T))
            drawManipulateDesc.Operation = ImGuizmo::UNIVERSAL;

        if (ImGui::IsKeyPressed(ImGuiKey_X))
        {
            if (drawManipulateDesc.Mode == ImGuizmo::MODE::LOCAL)
            {
                drawManipulateDesc.Mode = ImGuizmo::MODE::WORLD;
            }
            else
            {
                drawManipulateDesc.Mode = ImGuizmo::MODE::LOCAL;
            }
        }      
    }  
}

void EditorSceneTool::DrawManipulate() 
{
    if (false == _manipulateObject.expired())
    {
        auto pObject = _manipulateObject.lock();
        Matrix worldMatrix = pObject->transform->GetWorldMatrix();
        Matrix* pObjectMatrix = &worldMatrix;

        EditorDynamicCamera* pDynamicCamera = _camera.get();

        drawManipulateDesc.ViewDesc.ClientRight = _clientRight;
        drawManipulateDesc.ViewDesc.ClientTop   = _clientTop;

        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(_clientLeft, _clientTop, _clientWidth, _clientHeight);
        _isUseManipulate = ImGuiHelper::DrawManipulate(pDynamicCamera, pObjectMatrix, drawManipulateDesc);
        _isUsing = ImGuizmo::IsUsing();
        _isOver  = ImGuizmo::IsOver();

        if (true == _isUseManipulate)
        {         
            Transform* parent = pObject->transform->Parent;
            Vector3    position;
            Quaternion rotation;
            Vector3    scale;
            if (nullptr == parent)
            {
                worldMatrix.Decompose(scale, rotation, position);
            }
            else
            {
                const Matrix& parentWorldInvert = parent->GetWorldMatrix().Invert();
                Matrix localMatrix = worldMatrix * parentWorldInvert;
                localMatrix.Decompose(scale, rotation, position);
            }
            pObject->transform->Position = position;
            pObject->transform->Rotation = rotation;
            pObject->transform->Scale    = scale;
        }

        static bool prevUseManipulate = false;
        static ManipulateCommand::Transform prevTransform;
        if (prevUseManipulate != _isUseManipulate)
        {
            if (true == _isUseManipulate)
            {
                _isStartManipulate = true;
                prevTransform.Position = pObject->transform->Position;
                prevTransform.Rotation = pObject->transform->Rotation;
                prevTransform.Scale    = pObject->transform->Scale;
            }
            else
            {
                ManipulateCommand::Transform currTransform;
                currTransform.Position = pObject->transform->Position;
                currTransform.Rotation = pObject->transform->Rotation;
                currTransform.Scale    = pObject->transform->Scale;
                _isEndManipulate = true;
                UmCommandManager.Do<ManipulateCommand>(pObject, currTransform, prevTransform);
            }
        }
        else
        {
            _isStartManipulate = false;
            _isEndManipulate   = false;
        }
        prevUseManipulate = _isUseManipulate;
    }   
}

void EditorSceneTool::DrawSceneView() 
{
    auto   handle = UmRenderer.GetRenderSceneImage("Editor");
    ImGui::Image((ImTextureID)handle.ptr, {_clientWidth, _clientHeight});
}

void EditorSceneTool::SerializedReflectEvent() 
{
    Vector3 camPos = _camera->GetPosition();
    std::memcpy(ReflectFields->CameraPosition.data(), &camPos, sizeof(ReflectFields->CameraPosition));

    Quaternion camRot = _camera->GetRotation();
    camRot.Normalize();
    std::memcpy(ReflectFields->CameraRotation.data(), &camRot, sizeof(ReflectFields->CameraRotation));

    UpdateCameraSetting();
}

void EditorSceneTool::DeserializedReflectEvent() 
{
    Vector3 camPos = Vector3(ReflectFields->CameraPosition.data());
    _camera->SetPosition(camPos);

    Quaternion camRot = Quaternion(ReflectFields->CameraRotation.data());
    if (camRot.w <= Mathf::AngleEpsilon)
    {
        camRot = Quaternion();
    }
    camRot.Normalize();
    _camera->SetRotation(camRot);

    UpdateCameraSetting();
}

void EditorSceneTool::UpdateCameraSetting() 
{
    _camera->SetMoveSpeed(ReflectFields->CameraMoveSpeed);
    _camera->SetRotationSpeed(ReflectFields->CameraRotateSpeed / 1000.f);
}

EditorSceneTool::ManipulateCommand::ManipulateCommand(
    const std::shared_ptr<GameObject>& target, 
    ManipulateCommand::Transform& curr,
    ManipulateCommand::Transform& prev) 
    : 
    UmCommand("Manipulate"),
    _target(target),
    _curr(curr),
    _prev(prev)
{

}

EditorSceneTool::ManipulateCommand::~ManipulateCommand() = default;

void EditorSceneTool::ManipulateCommand::Execute() 
{
    if (false == _target.expired())
    {
        auto object = _target.lock();
        object->transform->Position = _curr.Position;
        object->transform->Rotation = _curr.Rotation;
        object->transform->Scale    = _curr.Scale;
    }
}

void EditorSceneTool::ManipulateCommand::Undo() 
{
    if (false == _target.expired())
    {
        auto object = _target.lock();
        object->transform->Position = _prev.Position;
        object->transform->Rotation = _prev.Rotation;
        object->transform->Scale    = _prev.Scale;
    }
}
