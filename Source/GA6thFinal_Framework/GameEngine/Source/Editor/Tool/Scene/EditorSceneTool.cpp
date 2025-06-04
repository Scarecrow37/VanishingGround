#include "pch.h"
#include "Editor/Tool/Scene/Command/EditorSceneCommands.h"
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

    _drawManipulateDesc.Operation = ImGuizmo::TRANSLATE;
    _drawManipulateDesc.Mode      = ImGuizmo::MODE::WORLD;
    _drawManipulateDesc.UseSnap = false;
    _drawManipulateDesc.Snap = {1.f, 1.f, 1.f};

    _drawManipulateDesc.ViewDesc.Size = ImVec2(128, 128);
    _drawManipulateDesc.ViewDesc.BackgroundColor = 0x10101010;
    UpdateCameraSetting();
}

EditorSceneTool::~EditorSceneTool()
{

}

void EditorSceneTool::SetManipulateObject(std::weak_ptr<GameObject>& object) 
{
    pSceneTool->_manipulateObject = object;
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
    _window = ImGui::GetCurrentWindow();
    DragDropEvent();
    SetCamera();    
    DrawSceneView();
    DrawManipulate();
}

void EditorSceneTool::OnFrameEnd()
{
}

void EditorSceneTool::OnFrameFocusStay()
{
    _camera->Update();
    UpdateMode();
}
    
void EditorSceneTool::DragDropEvent() 
{
    namespace fs = std::filesystem;
    ImRect rect  = _window->Rect();
    if (ImGui::BeginDragDropTargetCustom(rect, _window->ID))
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
        {
            DragDropAsset::Data* data = (DragDropAsset::Data*)payload->Data;
            std::weak_ptr<File::Context>* wpContext = data->pContext;
            if (false == wpContext->expired())
            {
                auto              context   = wpContext->lock();
                const File::Path& path      = context->GetPath();
                fs::path extension = path.extension();
            
                if (".hdr" == extension)
                {
                    UmSceneManager.SetSkyBox(path);
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
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
    _clientHeight = std::max(_clientHeight, Mathf::Epsilon);
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
            _drawManipulateDesc.Operation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_E))
            _drawManipulateDesc.Operation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R))
            _drawManipulateDesc.Operation = ImGuizmo::SCALE;
        if (ImGui::IsKeyPressed(ImGuiKey_T))
            _drawManipulateDesc.Operation = ImGuizmo::UNIVERSAL;

        if (ImGui::IsKeyPressed(ImGuiKey_X))
        {
            if (_drawManipulateDesc.Mode == ImGuizmo::MODE::LOCAL)
            {
                _drawManipulateDesc.Mode = ImGuizmo::MODE::WORLD;
            }
            else
            {
                _drawManipulateDesc.Mode = ImGuizmo::MODE::LOCAL;
            }
        }      
    }  
}

void EditorSceneTool::DrawManipulate() 
{
    if (false == _manipulateObject.expired())
    {
        auto pObject = _manipulateObject.lock();
        if (pObject->IsValid())
        {
            bool isLeftShiftHold = ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftShift);

            Matrix  worldMatrix   = pObject->transform->GetWorldMatrix();
            Matrix* pObjectMatrix = &worldMatrix;

            EditorDynamicCamera* pDynamicCamera = _camera.get();

            _drawManipulateDesc.ViewDesc.ClientRight = _clientRight;
            _drawManipulateDesc.ViewDesc.ClientTop   = _clientTop;

            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(_clientLeft, _clientTop, _clientWidth, _clientHeight);
            _isUseManipulate = ImGuiHelper::DrawManipulate(pDynamicCamera, pObjectMatrix, _drawManipulateDesc);
            _isUsing         = ImGuizmo::IsUsing();
            _isOver          = ImGuizmo::IsOver();

            if (isLeftShiftHold)
            {
                if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_D))
                {
                    UmCommandManager.Do<Command::EditorScene::DuplicateCommand>(pObject.get());
                }
            }

            if (IsFocusFrame())
            {
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
                        Matrix        localMatrix       = worldMatrix * parentWorldInvert;
                        localMatrix.Decompose(scale, rotation, position);
                    }
                    pObject->transform->Position = position;
                    pObject->transform->Rotation = rotation;
                    pObject->transform->Scale    = scale;
                }

                static bool                         prevIsUsing = false;
                static ManipulateCommand::Transform prevTransform;
                if (prevIsUsing != _isUsing)
                {
                    if (true == _isUsing)
                    {
                        _isUsingStart          = true;
                        prevTransform.Position = pObject->transform->Position;
                        prevTransform.Rotation = pObject->transform->Rotation;
                        prevTransform.Scale    = pObject->transform->Scale;
                    }
                    else
                    {
                        _isUsingEnd = true;
                        ManipulateCommand::Transform currTransform;
                        currTransform.Position = pObject->transform->Position;
                        currTransform.Rotation = pObject->transform->Rotation;
                        currTransform.Scale    = pObject->transform->Scale;
                        UmCommandManager.Do<ManipulateCommand>(pObject, currTransform, prevTransform);
                    }
                }
                else
                {
                    _isUsingStart = false;
                    _isUsingEnd   = false;
                }
                prevIsUsing = _isUsing;

                if (isLeftShiftHold)
                {
                    if (_isUsingStart)
                    {
                        UmCommandManager.Do<Command::EditorScene::DuplicateCommand>(pObject.get());
                    }
                }
            }
        }
    }   
}

void EditorSceneTool::DrawSceneView() 
{
    D3D12_GPU_DESCRIPTOR_HANDLE handle = UmRenderer.GetRenderSceneImage("Editor");
    ImGui::Image((ImTextureID)handle.ptr, {_clientWidth, _clientHeight});  

    constexpr ImVec2 iconButtonSize = ImVec2(64.0f, 64.0f);
    constexpr ImVec2 damp = ImVec2(4.f, 4.f);
    ImVec2 moveIconPos = _window->ContentRegionRect.Min;
    ImGui::SetCursorScreenPos(ImVec2(moveIconPos.x + damp.x, moveIconPos.y + damp.y));
    //static std::shared_ptr<Texture> moveIconTexture = UmResourceManager.LoadResource<Texture>(L"../GameEngine/Icon/Editor/Move.png");
    //static D3D12_GPU_DESCRIPTOR_HANDLE moveIconHandle = moveIconTexture->GetHandle();
    //ImGui::ImageButton(
    //"Move",
    //(ImTextureID)moveIconHandle.ptr,
    //iconButtonSize,
    //ImVec2(0,0),
    //ImVec2(1,1),
    //ImVec4(0,0,0,0),
    //ImVec4(1,1,1,1)
    //);

    auto ImageButtonOperation = [&](ImGuizmo::OPERATION op) 
    {
        bool isActive = IsActiveOperation(op);
        if (isActive)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.22f, 0.28f, 0.35f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.32f, 0.40f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.18f, 0.24f, 0.30f, 1.0f));
        }

        if (ImGuizmo::TRANSLATE == op)
        {
            if (ImGui::Button("Move", iconButtonSize))
            {
                _drawManipulateDesc.Operation = ImGuizmo::TRANSLATE;
            }
        }
        else if (ImGuizmo::ROTATE == op)
        {
            if (ImGui::Button("Rotation", iconButtonSize))
            {
                _drawManipulateDesc.Operation = ImGuizmo::ROTATE;
            }
        }
        else if (ImGuizmo::SCALE == op)
        {
            if (ImGui::Button("Scale", iconButtonSize))
            {
                _drawManipulateDesc.Operation = ImGuizmo::SCALE;
            }
        }
        else if (ImGuizmo::UNIVERSAL == op)
        {
            if (ImGui::Button("Transform", iconButtonSize))
            {
                _drawManipulateDesc.Operation = ImGuizmo::UNIVERSAL;
            }
        }

        if (isActive)
        {
            ImGui::PopStyleColor(3);
        }
    };

    auto ImageButtonMode = [&]() 
    {
        bool isWorldMode = IsActiveMode(ImGuizmo::MODE::WORLD);
        if (isWorldMode)
        {
            if (ImGui::Button("World", iconButtonSize))
            {
                _drawManipulateDesc.Mode = ImGuizmo::MODE::LOCAL;
            }
        }
        else
        {
            if (ImGui::Button("Local", iconButtonSize))
            {
                _drawManipulateDesc.Mode = ImGuizmo::MODE::WORLD;
            }
        }
    };
   
    ImageButtonOperation(ImGuizmo::OPERATION::TRANSLATE);
    ImGui::SameLine();
    ImageButtonOperation(ImGuizmo::OPERATION::ROTATE);
    ImGui::SameLine();
    ImageButtonOperation(ImGuizmo::OPERATION::SCALE);
    ImGui::SameLine();
    ImageButtonOperation(ImGuizmo::OPERATION::UNIVERSAL);
    ImGui::SameLine();
    ImageButtonMode();

}

bool EditorSceneTool::IsActiveOperation(ImGuizmo::OPERATION op) const
{
    return op == _drawManipulateDesc.Operation;
}

bool EditorSceneTool::IsActiveMode(ImGuizmo::MODE mode) const
{
    return mode == _drawManipulateDesc.Mode;
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
    _camera->Update();
}

void EditorSceneTool::UpdateCameraSetting() 
{
    _camera->SetMoveSpeed(ReflectFields->CameraMoveSpeed);
    _camera->SetRotationSpeed(ReflectFields->CameraRotateSpeed);
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
