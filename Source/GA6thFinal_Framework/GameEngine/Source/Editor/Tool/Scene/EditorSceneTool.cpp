#include "pch.h"
#include "Editor/Tool/Hierarchy/EditorHierarchyTool.h"
#include "Editor/Tool/Hierarchy/Command/FocusCommand.h"
#include "Editor/Tool/Scene/Command/EditorSceneCommands.h"
#include "Editor/DynamicCamera/EditorDynamicCamera.h"
#include "EditorSceneTool.h"
#include "Mesh/MeshComponent.h"
#include "GraphicsEngine/Interface/IMeshRenderer.h"

REFLECT_FUNCTION(EditorSceneTool)

using namespace u8_literals;

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
    SetImGuiWindowFlag(ImGuiWindowFlags_NoScrollbar);

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

void EditorSceneTool::SetManipulateObject(std::weak_ptr<GameObject> object) 
{
    pSceneTool->_manipulateObject = object;
}

const Matrix& EditorSceneTool::GetCameraMatrix()
{
    return _camera->GetCamera()->GetWorldMatrix();
}

void EditorSceneTool::SetCameraToObject(std::weak_ptr<GameObject> destination) 
{
    if (false == destination.expired())
    {
        auto pObject = destination.lock();
        if (pObject->IsValid() && _camera)
        {
            _isFocusedCamera = true;
            const Matrix& world = pObject->transform->GetWorldMatrix();
            _focusedCameraTargetPosition = world.Translation();
            _focusedCameraStartPosition  = _camera->GetPivotPosition();
        }
    }
}

void EditorSceneTool::OnTickGui()
{
    if (_isFocusedCamera && _camera)
    {
        const float lerpT = _focusedLerpScale;
        Vector3& current  = _focusedCameraStartPosition;
        Vector3& target   = _focusedCameraTargetPosition;

        current = ImLerp(current, target, lerpT);

        // 거리가 충분히 가까워지면 카메라를 고정합니다.
        Vector3 delta = target - current;
        float   deltaLength = delta.Length();
        if (deltaLength <= 1.0f)
        {
            current          = target;
            _isFocusedCamera = false;
        }
        _camera->SetPivotPosition(current);
        _camera->Update();
    }
}

void EditorSceneTool::OnStartGui()
{
    std::shared_ptr<Camera> camera = UmGraphics.GetCamera("Editor");
    assert(nullptr != camera && L"Camera is nullptr");

    _camera->SetTarget(camera);
    _dockWindow = GetOwnerDockWindow();
    _editorHierarchyTool = _dockWindow->GetGui<EditorHierarchyTool>();

    LoadDefaultIcon();
}

void EditorSceneTool::OnPreFrameBegin()
{
    SetMoveFlag();
}

void EditorSceneTool::OnPostFrameBegin()
{
    _isHoveredWindow = ImGui::IsWindowHovered();
}

void EditorSceneTool::OnFrameRender() 
{
    EGizmoManager& gizmoManager = UmGizmoManager;
    _window = ImGui::GetCurrentWindow();
    gizmoManager.BeginDraw(_window, _camera->GetCamera().get());
    {
        if (_isHoveredWindow)
        {
            if (ImGui::IsKeyPressed(ImGuiKey_MouseRight, false))
            {
                ImGui::SetWindowFocus();
            }
        }
        DragDropEvent();
        SetCamera();
        DrawSceneView();
        DrawManipulate();
        if (ReflectFields->DrawGizmo)
        {
            gizmoManager.DrawImGuizmo(_drawManipulateDesc);
            bool enableGizmoButton = false == gizmoManager.IsOverImGuizmo();
            enableGizmoButton &= false == _isOver;
            gizmoManager.Draw(enableGizmoButton);
        }
        RayPicker();
        VertexSnap();
        UpdateKeyboardFrameRender();
    }
    UmGizmoManager.EndDraw();
}

void EditorSceneTool::OnFrameEnd()
{
}

void EditorSceneTool::OnFrameFocusStay()
{
    if (IsFocusFrame())
    {
        _camera->Update(_isHoveredWindow);
    }   
    UpdateKeyboardFrameFocus();
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
            File::Path path = data->GetPath();
            fs::path extension = path.extension();
            if (".hdr" == extension)
            {
                UmSceneManager.SetSkyBox(path);
                UmSceneManager.SetSkyIBL(path);
            }
        }
        ImGui::EndDragDropTarget();
    }
}

void EditorSceneTool::SetMoveFlag()
{
    RemoveImGuiWindowFlag(ImGuiWindowFlags_NoMove);
    if (true == _isOver)
    {
        AddImGuiWindowFlag(ImGuiWindowFlags_NoMove);
    }
}

void EditorSceneTool::SetCamera()
{
    ImVec2 windowPos  = ImGui::GetWindowPos();
    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();

    _sceneClienttLeft   = windowPos.x + contentMin.x;
    _sceneClientRight  = windowPos.x + contentMax.x;
    _sceneClientTop    = windowPos.y + contentMin.y;
    _sceneClientBottom = windowPos.y + contentMax.y;

    _sceneClientWidth  = _sceneClientRight - _sceneClienttLeft;
    _sceneClientHeight = _sceneClientBottom - _sceneClientTop;
    _sceneClientHeight = std::max(_sceneClientHeight, Mathf::Epsilon);
    ReflectFields->CameraAspect = _sceneClientWidth / _sceneClientHeight;

    ReflectFields->CameraFov   = std::max(ReflectFields->CameraFov, 5.f);
    ReflectFields->CameraNearZ = std::max(ReflectFields->CameraNearZ, 0.1f);
    ReflectFields->CameraFarZ  = std::max(ReflectFields->CameraFarZ, 10.f);

    auto& camera = _camera->GetCamera();
    camera->SetupPerspective(
        ReflectFields->CameraFov,
        ReflectFields->CameraAspect,
        ReflectFields->CameraNearZ,
        ReflectFields->CameraFarZ);
}

void EditorSceneTool::LoadDefaultIcon() 
{
    constexpr auto icons = rfl::get_enumerator_array<SceneGizmo::DefaultIcon>();
    static std::array<std::shared_ptr<Texture>, icons.size()> iconTexture;
    int i = 0;
    for (auto& [key, data] : icons)
    {
        iconTexture[i] = UmResourceManager->LoadResource<Texture>(SceneGizmo::GetIconPath(data));
        i++;
    }
}

void EditorSceneTool::UpdateKeyboardFrameFocus()
{
    if (true == _isDrawedManipulate)
    {
        if (false == ImGui::IsKeyDown(ImGuiKey_MouseRight))
        {
            if (ImGui::IsKeyPressed(ImGuiKey_W, false))
                _drawManipulateDesc.Operation = ImGuizmo::TRANSLATE;
            if (ImGui::IsKeyPressed(ImGuiKey_E, false))
                _drawManipulateDesc.Operation = ImGuizmo::ROTATE;
            if (ImGui::IsKeyPressed(ImGuiKey_R, false))
                _drawManipulateDesc.Operation = ImGuizmo::SCALE;
            if (ImGui::IsKeyPressed(ImGuiKey_T, false))
                _drawManipulateDesc.Operation = ImGuizmo::UNIVERSAL;

            if (ImGui::IsKeyPressed(ImGuiKey_X, false))
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
}

void EditorSceneTool::UpdateKeyboardFrameRender() 
{
    if (_isDrawedManipulate)
    {
        if (_editorHierarchyTool->IsFocusFrame() || IsFocusFrame())
        {
            if (ImGui::IsKeyPressed(ImGuiKey_F, false))
            {
                auto& wPtrFocused = EditorHierarchyTool::GetFocusObject();
                SetCameraToObject(wPtrFocused);
            }
        }
    }
}

void EditorSceneTool::DrawManipulate() 
{
    _isDrawedManipulate = false == _manipulateObject.expired();
    if (true == _isDrawedManipulate)
    {
        auto pObject = _manipulateObject.lock();
        if (pObject->IsValid())
        {
            const ImGuiIO& io    = ImGui::GetIO();
            bool isLeftShiftHold = ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftShift);
            bool isMouseMoved    = (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f);

            Matrix  worldMatrix   = pObject->transform->GetWorldMatrix();
            Matrix* pObjectMatrix = &worldMatrix;

            EditorDynamicCamera* pDynamicCamera = _camera.get();

            _drawManipulateDesc.ViewDesc.ClientRight = _sceneClientRight;
            _drawManipulateDesc.ViewDesc.ClientTop   = _sceneClientTop;

            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(_sceneClienttLeft, _sceneClientTop, _sceneClientWidth, _sceneClientHeight);
            ImGuizmo::PushID(pObjectMatrix);
            _isUseManipulate = ImGuiHelper::DrawManipulate(pDynamicCamera, pObjectMatrix, _drawManipulateDesc);
            _isUsing         = ImGuizmo::IsUsing();
            _isOver          = ImGuizmo::IsOver();
            ImGuizmo::PopID();

            // 마우스를 움직인 경우에만 Moved 플래그 설정
            if (true == _isUsing && true == isMouseMoved)
            {
                _isMovedManipulate = true;
            }

            if (isLeftShiftHold)    
            {
                if (IsFocusFrame() && ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_D, false))
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
                    bool decomposeResult = false;
                    if (nullptr == parent)
                    {
                        decomposeResult = worldMatrix.Decompose(scale, rotation, position);
                    }
                    else
                    {
                        const Matrix& parentWorldInvert = parent->GetWorldMatrix().Invert();
                        Matrix localMatrix = worldMatrix * parentWorldInvert;
                        decomposeResult = localMatrix.Decompose(scale, rotation, position);
                    }
                    if (decomposeResult)
                    {
                        pObject->transform->Position = position;
                        pObject->transform->Rotation = rotation;
                        pObject->transform->Scale    = scale;
                    }
                }

                static bool                         prevIsUsing = false;
                static ManipulateCommand::Transform prevTransform;
                if (prevIsUsing != _isUsing)
                {
                    if (true == _isUsing)
                    {
                        _isUsingStart = true;
                        prevTransform.Position = pObject->transform->Position;
                        prevTransform.Rotation = pObject->transform->Rotation;
                        prevTransform.Scale    = pObject->transform->Scale;
                    }
                    else
                    {
                        // 이동한 경우에만 커맨드를 실행
                        if (true == _isMovedManipulate)
                        {
                            ManipulateCommand::Transform currTransform;
                            currTransform.Position = pObject->transform->Position;
                            currTransform.Rotation = pObject->transform->Rotation;
                            currTransform.Scale    = pObject->transform->Scale;
                            UmCommandManager.Do<ManipulateCommand>(pObject, currTransform, prevTransform);
                            _isMovedManipulate = false;
                        }
                        _isUsingEnd = true;
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
    else
    {
        _isUseManipulate    = false;
        _isDrawedManipulate = false;
        _isMovedManipulate  = false;
        _isUsingStart       = false;
        _isUsingEnd         = false;
        _isUsing            = false;
        _isOver             = false;            
    }
}

void EditorSceneTool::DrawSceneView() 
{
    D3D12_GPU_DESCRIPTOR_HANDLE handle = UmGraphics.GetRenderSceneImage("Editor");
    ImGui::Image((ImTextureID)handle.ptr, {_sceneClientWidth, _sceneClientHeight});  

    constexpr ImVec2 iconButtonSize = ImVec2(54.f, 54.f);
    constexpr ImVec2 damp = ImVec2(4.f, 4.f);
    ImVec2 moveIconPos = _window->ContentRegionRect.Min;
    ImGui::SetCursorScreenPos(ImVec2(moveIconPos.x + damp.x, moveIconPos.y + damp.y));
    
    static std::shared_ptr<Texture> moveIconTexture = UmResourceManager->LoadResource<Texture>(L"../GameEngine/Icon/Editor/Move.png");
    static const D3D12_GPU_DESCRIPTOR_HANDLE& moveIconHandle = moveIconTexture->GetGPUHandle();

    static std::shared_ptr<Texture> rotationIconTexture = UmResourceManager->LoadResource<Texture>(L"../GameEngine/Icon/Editor/Rotate.png");
    static const D3D12_GPU_DESCRIPTOR_HANDLE& rotationIconHandle = rotationIconTexture->GetGPUHandle();

    static std::shared_ptr<Texture> scaleIconTexture = UmResourceManager->LoadResource<Texture>(L"../GameEngine/Icon/Editor/Scale.png");
    static const D3D12_GPU_DESCRIPTOR_HANDLE& scaleIconHandle = scaleIconTexture->GetGPUHandle();

    static std::shared_ptr<Texture> transformIconTexture = UmResourceManager->LoadResource<Texture>(L"../GameEngine/Icon/Editor/Transform.png");
    static const D3D12_GPU_DESCRIPTOR_HANDLE& transformIconHandle = transformIconTexture->GetGPUHandle();

    static std::shared_ptr<Texture> worldIconTexture = UmResourceManager->LoadResource<Texture>(L"../GameEngine/Icon/Editor/World.png");
    static const D3D12_GPU_DESCRIPTOR_HANDLE& worldIconHandle = worldIconTexture->GetGPUHandle();

    static std::shared_ptr<Texture> localIconTexture = UmResourceManager->LoadResource<Texture>(L"../GameEngine/Icon/Editor/Local.png");
    static const D3D12_GPU_DESCRIPTOR_HANDLE& localIconHandle = localIconTexture->GetGPUHandle();

    static std::shared_ptr<Texture> gridSnapIconTexture = UmResourceManager->LoadResource<Texture>(L"../GameEngine/Icon/Editor/GridSnap.png");
    static const D3D12_GPU_DESCRIPTOR_HANDLE& gridSnapIconHandle = gridSnapIconTexture->GetGPUHandle();

    static std::shared_ptr<Texture> toggleLeftTexture = UmResourceManager->LoadResource<Texture>(L"../GameEngine/Icon/Editor/ToggleLeft.png");
    static const D3D12_GPU_DESCRIPTOR_HANDLE& toggleLeftHandle = toggleLeftTexture->GetGPUHandle();

    static std::shared_ptr<Texture> toggleRightTexture = UmResourceManager->LoadResource<Texture>(L"../GameEngine/Icon/Editor/ToggleRight.png");
    static const D3D12_GPU_DESCRIPTOR_HANDLE& toggleRightHandle = toggleRightTexture->GetGPUHandle();

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
            if (ImGui::ImageButton("Move", (ImTextureID)moveIconHandle.ptr, iconButtonSize))
            {
                _drawManipulateDesc.Operation = ImGuizmo::TRANSLATE;
            }
            ImGuiHelper::HoveredToolTip("Short cut: w\nMove");
        }
        else if (ImGuizmo::ROTATE == op)
        {
            if (ImGui::ImageButton("Rotation", (ImTextureID)rotationIconHandle.ptr, iconButtonSize))
            {
                _drawManipulateDesc.Operation = ImGuizmo::ROTATE;
            }
            ImGuiHelper::HoveredToolTip("Short cut: e\nRotation");
        }
        else if (ImGuizmo::SCALE == op)
        {
            if (ImGui::ImageButton("Scale", (ImTextureID)scaleIconHandle.ptr, iconButtonSize))
            {
                _drawManipulateDesc.Operation = ImGuizmo::SCALE;
            }
            ImGuiHelper::HoveredToolTip("Short cut: r\nScale");
        }
        else if (ImGuizmo::UNIVERSAL == op)
        {
            if (ImGui::ImageButton("Transform", (ImTextureID)transformIconHandle.ptr, iconButtonSize))
            {
                _drawManipulateDesc.Operation = ImGuizmo::UNIVERSAL;
            }
            ImGuiHelper::HoveredToolTip("Short cut: t\nTransform");
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
            if (ImGui::ImageButton("World", (ImTextureID)worldIconHandle.ptr, iconButtonSize))
            {
                _drawManipulateDesc.Mode = ImGuizmo::MODE::LOCAL;
            }
            ImGuiHelper::HoveredToolTip("Short cut: x\nLocal");
        }
        else
        {
            if (ImGui::ImageButton("Local", (ImTextureID)localIconHandle.ptr, iconButtonSize))
            {
                _drawManipulateDesc.Mode = ImGuizmo::MODE::WORLD;
            }
            ImGuiHelper::HoveredToolTip("Short cut : x\nWorld");
        }
    };
    auto ImageButtonGridSnap = [&]() 
    {
        bool isActive = _drawManipulateDesc.UseSnap;
        isActive |= ReflectFields->VertexSnapUse;
        if (isActive)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.22f, 0.28f, 0.35f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.32f, 0.40f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.18f, 0.24f, 0.30f, 1.0f));
        }

        ImGui::ImageButton("Grid snap", (ImTextureID)gridSnapIconHandle.ptr, iconButtonSize);
        ImGuiHelper::HoveredToolTip(u8"우클릭을 통해 수치를 변경할 수 있습니다."_c_str);   
        if (ImGui::BeginPopupContextItem("Snap setting"))
        {
            ImGui::Checkbox("Grid snap", &_drawManipulateDesc.UseSnap);
            ImGui::DragFloat3("Step", _drawManipulateDesc.Snap.data(), 0.1f);
            ImGui::Separator();

            if (ImGui::Checkbox("Vertex snap", &ReflectFields->VertexSnapUse))
            {
                if (false == ReflectFields->VertexSnapUse)
                {
                    _isSnapping = false;
                }          
            }        
            ImGui::DragFloat("Threshold", &ReflectFields->VertexSnapThreshold, 0.01f);

            ImGui::EndPopup();
        }

        if (isActive)
        {
            ImGui::PopStyleColor(3);
        }
    };
    
    static bool showSettings = true;
    auto ImageButtonToggleSetting = [&]() 
    {
        if (true == showSettings)
        {
            if (ImGui::ImageButton("left", (ImTextureID)toggleLeftHandle.ptr, iconButtonSize))
            {
                showSettings = !showSettings;
            }
        }
        else
        {
            if (ImGui::ImageButton("right", (ImTextureID)toggleRightHandle.ptr, iconButtonSize))
            {
                showSettings = !showSettings;
            }
        }
    };
   
    if (showSettings)
    {
        ImageButtonMode();
        ImGui::SameLine();
        ImageButtonOperation(ImGuizmo::OPERATION::TRANSLATE);
        ImGui::SameLine();
        ImageButtonOperation(ImGuizmo::OPERATION::ROTATE);
        ImGui::SameLine();
        ImageButtonOperation(ImGuizmo::OPERATION::SCALE);
        ImGui::SameLine();
        ImageButtonOperation(ImGuizmo::OPERATION::UNIVERSAL);
        ImGui::SameLine();
        ImageButtonGridSnap();
        ImGui::SameLine();
    }
    ImageButtonToggleSetting();
    if (_camera && showSettings)
    {
        float moveSpeed     = _camera->GetMoveSpeed();
        float rotationSpeed = _camera->GetRotationSpeed();
        float pivotDistance = _camera->GetPivot();
        int   pushCount     = 0;
        // 움직인 경우에만 알파를 낮춤
        if (true == ImGui::IsKeyDown(ImGuiKey_MouseRight) && IsFocusFrame())
        {
            ImGuiStyle& style   = ImGui::GetStyle();
            ImVec4      bgCol   = style.Colors[ImGuiCol_FrameBg];
            ImVec4      textCol = style.Colors[ImGuiCol_Text];
            bgCol.w *= 0.3f;
            textCol.w *= 0.3f;
            ImGui::PushStyleColor(ImGuiCol_FrameBg, bgCol);
            ++pushCount;
            ImGui::PushStyleColor(ImGuiCol_Text, textCol);
            ++pushCount;
        }
        ImGui::SetNextItemWidth(150.0f);
        int flags = ImGuiSliderFlags_AlwaysClamp;
        ImGui::SliderFloat("Camera Move Speed##move speed",
            &moveSpeed,
            _camera->GetMinMoveSpeed(), 
            _camera->GetMaxMoveSpeed(), 
            "%.2f", 
            flags
        );
        ImGui::SetNextItemWidth(150.0f);
        ImGui::SliderFloat("Camera Rotation Speed##rotation speed",
            &rotationSpeed, 
            _camera->GetMinRotationSpeed(),
            _camera->GetMaxRotationSpeed(),
            "%.2f",
            flags
        );
        ImGui::SetNextItemWidth(150.0f);
        ImGui::DragFloat("Camera Pivot Distance##pivot distance",
            &pivotDistance,
            0.1f,
            -100000.0f,
            100000.0f
        );
        ImGui::PopStyleColor(pushCount);
        _camera->SetMoveSpeed(moveSpeed);
        _camera->SetRotationSpeed(rotationSpeed);
        _camera->SetPivot(pivotDistance);
        UpdateReflectFields();
    }

    if (ReflectFields->DrawGizmo)
    {
        const auto& runtimeObjects = ESceneManager::Engine::GetRuntimeObjects();
        auto        focusObject    = EditorHierarchyTool::GetFocusObject().lock();
        for (auto& object : runtimeObjects)
        {
            if (object && object->IsValid())
            {
                for (size_t i = 0; i < object->GetComponentCount(); ++i)
                {
                    Component* component = object->GetComponentAtIndex<Component>(i);
                    if (component)
                    {
                        if (nullptr == focusObject || object != focusObject)
                        {
                            component->OnDrawDebug();
                        }
                    }
                }
            }
        }
    }   
}

void EditorSceneTool::RayPicker() 
{
    bool isSceneGuizmoUsing = EGizmoManager::GuizmoState::IDLE != UmGizmoManager.GetImGuizmoState();
    if (false == _isOver && 
        false == _isUsingStart && 
        false == _isUsingEnd &&
        false == isSceneGuizmoUsing
        )
    {
        bool isLeftAltDown = ImGui::IsKeyDown(ImGuiKey_LeftAlt);
        if (false == isLeftAltDown && IsFocusFrame() && _isHoveredWindow && ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
        {
            ImGuiIO& io = ImGui::GetIO();
            if (io.MousePos.x >= _sceneClienttLeft && io.MousePos.y >= _sceneClientTop &&
                io.MousePos.x <= _sceneClientRight && io.MousePos.y <= _sceneClientBottom)
            {
                ImVec2        screenMousePos = io.MousePos - _window->ContentRegionRect.Min;
                float         ndcX           = (2.0f * screenMousePos.x) / _sceneClientWidth - 1.0f;
                float         ndcY           = 1.0f - (2.0f * screenMousePos.y) / _sceneClientHeight;
                Vector4       ndcMin(ndcX, ndcY, 0.f, 1.f);
                Vector4       ndcMax(ndcX, ndcY, 1.f, 1.f);
                const Matrix& inversProjec = _camera->GetCamera()->GetProjectionInverseMatrix();
                Matrix        inversView   = _camera->GetCamera()->GetViewMatrix().Invert();
                Vector4       rayMin       = Vector4::Transform(ndcMin, inversProjec);
                Vector4       rayMax       = Vector4::Transform(ndcMax, inversProjec);
                rayMin /= rayMin.w;
                rayMax /= rayMax.w;
                rayMin         = Vector4::Transform(rayMin, inversView);
                rayMax         = Vector4::Transform(rayMax, inversView);
                Vector3 rayPos = Vector3(rayMin);
                Vector3 rayDir = Vector3(rayMax) - Vector3(rayMin);
                rayDir.Normalize();

                const std::vector<std::weak_ptr<MeshComponent>>& meshWeakComponents = UmSceneManager.GetMeshComponents();
                std::vector<std::shared_ptr<MeshComponent>> meshComponents;
                meshComponents.reserve(meshWeakComponents.size());
                for (auto& weakMesh : meshWeakComponents)
                {
                    if (std::shared_ptr<MeshComponent> mesh = weakMesh.lock())
                    {
                        if (mesh->gameObject->IsValid())
                        {
                            meshComponents.push_back(mesh);
                        }                       
                    }           
                }

                const Vector3& camPosition = _camera->GetPosition();
                std::ranges::sort(meshComponents, 
                    [&camPosition](const std::shared_ptr<MeshComponent>& a, const std::shared_ptr<MeshComponent>& b) 
                    {
                        float disA = Vector3::DistanceSquared(camPosition, a->transform->Position);
                        float disB = Vector3::DistanceSquared(camPosition, b->transform->Position);
                        return disA < disB;
                    });

                bool intersects = false;
                for (auto& meshComponent : meshComponents)
                {
                    if (meshComponent->Enable && meshComponent->gameObject->ActiveInHierarchy)
                    {
                        if (meshComponent->Renderer)
                        {
                            const auto& model = meshComponent->Renderer->GetModel();
                            if (model)
                            {
                                auto& meshes = model->GetMeshes();
                                for (auto& baseMesh : meshes)
                                {
                                    const BoundingOrientedBox& obb = baseMesh->GetBoundingBox();
                                    BoundingOrientedBox        obbWorld;
                                    const Matrix& worldMatrix = meshComponent->gameObject->transform->GetWorldMatrix();
                                    obb.Transform(obbWorld, worldMatrix);

                                    float dist = 0.f;
                                    intersects = obbWorld.Intersects(rayPos, rayDir, dist);
                                    if (true == intersects)
                                    {
                                        std::weak_ptr old = EditorHierarchyTool::GetFocusObject();
                                        UmCommandManager.Do<Command::Hierarchy::FocusCommand>(
                                            old, meshComponent->gameObject->GetWeakPtr());
                                        break;
                                    }
                                }
                            }
                        }                     
                    }                       
                    
                    if (true == intersects)
                    {
                        break;
                    }
                }
            }
        }
    }
}

template <typename Func>
static void AutoVertexForeach(char* vertexBuff, unsigned int stride, unsigned int size, Func func, int maxForeachCount = 1000)
{
    if (stride == sizeof(StaticMeshVertex))
    {
        StaticMeshVertex* vertexes = reinterpret_cast<StaticMeshVertex*>(vertexBuff);
        for (size_t i = 0; i < size && i < maxForeachCount; ++i)
        {
            StaticMeshVertex& vertex = vertexes[i];
            func(vertex);
        }
    }
    else if (stride == sizeof(SkeletalMeshVertex))
    {
        SkeletalMeshVertex* vertexes = reinterpret_cast<SkeletalMeshVertex*>(vertexBuff);
        for (size_t i = 0; i < size && i < maxForeachCount; ++i)
        {
            SkeletalMeshVertex& vertex = vertexes[i];
            func(vertex);
        }
    }
}

void EditorSceneTool::VertexSnap() 
{
    if (false == ReflectFields->VertexSnapUse)
    {
        return;
    }

    //스넵을 탈출하기 위한 마우스 이동 임계값 입니다.
    static constexpr float mouseMoveDeltaThresHold = 50.f;

    static std::vector<MeshComponent*> manipulateMeshes;
    if (_isUsingStart)
    {
        auto manipulateObject = _manipulateObject.lock();
        manipulateMeshes.clear();
        if (nullptr != manipulateObject)
        {
            manipulateMeshes = manipulateObject->GetComponents<MeshComponent>();
        }
    }
    else if (_isUsingEnd)
    {
        manipulateMeshes.clear();
    }

    if (_isUsing && ReflectFields->VertexSnapThreshold)    
    {
        auto manipulateObject = _manipulateObject.lock();
        if (false == manipulateMeshes.empty())
        {
            if (true == _weakClosestMeshComponent.expired())
            {
                const std::vector<std::weak_ptr<MeshComponent>>& meshComponents = UmSceneManager.GetMeshComponents();
                const Matrix& manipulateMatrix = manipulateObject->transform->GetWorldMatrix();
                BoundingOrientedBox manipulateObbWorld;
                bool intersects = false;
                for (auto& manipulateMesh : manipulateMeshes)
                {
                    if (manipulateMesh->Enable && manipulateMesh->gameObject->ActiveInHierarchy)
                    {
                        if (nullptr != manipulateMesh->Renderer)
                        {
                            const auto& manipulateModel = manipulateMesh->Renderer->GetModel();
                            if (manipulateModel)
                            {
                                const auto& manipulateMeshes = manipulateModel->GetMeshes();
                                for (auto& manipulateMesh : manipulateMeshes)
                                {
                                    manipulateMesh->GetBoundingBox().Transform(manipulateObbWorld, manipulateMatrix);
                                    for (auto& weak : meshComponents)
                                    {
                                        if (false == weak.expired())
                                        {
                                            auto meshComponent = weak.lock();
                                            if (manipulateObject.get() == &meshComponent->gameObject)
                                            {
                                                continue;
                                            }
                                            if (false == meshComponent->Enable ||
                                                false == meshComponent->gameObject->ActiveInHierarchy)
                                            {
                                                continue;
                                            }

                                            if (nullptr != meshComponent->Renderer)
                                            {
                                                const Matrix& meshMatrix =
                                                    meshComponent->gameObject->transform->GetWorldMatrix();
                                                BoundingOrientedBox meshObbWorld;
                                                const auto&         model = meshComponent->Renderer->GetModel();
                                                if (model)
                                                {
                                                    const auto& meshes = model->GetMeshes();
                                                    for (auto& mesh : meshes)
                                                    {
                                                        mesh->GetBoundingBox().Transform(meshObbWorld, meshMatrix);
                                                        intersects = meshObbWorld.Intersects(manipulateObbWorld);

                                                        if (true == intersects)
                                                        {
                                                            _weakClosestMeshComponent = weak;
                                                            _closestBaseMesh          = mesh.get();
                                                            _manipulateBaseMesh       = manipulateMesh.get();
                                                            break;
                                                        }
                                                    }
                                                }
                                            }

                                            if (true == intersects)
                                                break;
                                        }
                                    }

                                    if (true == intersects)
                                        break;
                                }
                            }                      
                        }
                        if (true == intersects)
                            break;
                    }
                }
            }
            else
            {
                static float mouseMoveDelta = 0.f;
                static Vector3 snapPosition;
                if (false == _isSnapping)
                {

                    auto                closestMeshComponent = _weakClosestMeshComponent.lock();
                    Transform&          closestTransform     = closestMeshComponent->gameObject->transform;
                    BoundingOrientedBox closestObbWorld;
                    _closestBaseMesh->GetBoundingBox().Transform(closestObbWorld, closestTransform.GetWorldMatrix());

                    bool                intersects          = false;
                    Transform&          manipulateTransform = manipulateObject->transform;
                    BoundingOrientedBox manipulateObbWorld;
                    for (auto& meshComponent : manipulateMeshes)
                    {
                        if (nullptr != meshComponent->Renderer)
                        {
                            const auto& model = meshComponent->Renderer->GetModel();
                            if (model)
                            {
                                const auto& meshes = model->GetMeshes();
                                for (auto& mesh : meshes)
                                {
                                    mesh->GetBoundingBox().Transform(manipulateObbWorld,
                                                                     manipulateTransform.GetWorldMatrix());
                                    intersects = closestObbWorld.Intersects(manipulateObbWorld);
                                    if (true == intersects)
                                    {
                                        break;
                                    }
                                }
                            }
                        }
                        if (true == intersects)
                        {
                            break;
                        }
                    }
                    if (false == intersects)
                    {
                        _weakClosestMeshComponent = std::weak_ptr<MeshComponent>();
                        _closestBaseMesh          = nullptr;
                        _manipulateBaseMesh       = nullptr;
                    }

                    // Vertex snap
                    if (_manipulateBaseMesh && _closestBaseMesh)
                    {
                        auto PointInOrientedBox = [](const BoundingOrientedBox& box, const Vector3& point) {
                            Quaternion invRot = box.Orientation;
                            invRot.Conjugate();

                            Vector3 local = point - box.Center;
                            local         = Vector3::Transform(local, invRot);

                            return std::abs(local.x) <= box.Extents.x && std::abs(local.y) <= box.Extents.y &&
                                   std::abs(local.z) <= box.Extents.z;
                        };

                        static std::vector<Vector3> closestVertexes;
                        static std::vector<Vector3> manipulateVertexes;
                        if (false == _isSnapping)
                        {
                            closestVertexes.clear();
                            const Matrix& closestWorldMatrix =
                                closestMeshComponent->gameObject->transform->GetWorldMatrix();
                            char*        closestVertexBuff;
                            unsigned int closestStride;
                            unsigned int closestSize;
                            _closestBaseMesh->GetVertexInfo(closestVertexBuff, closestStride, closestSize);
                            // AABB culling
                            AutoVertexForeach(closestVertexBuff, closestStride, closestSize,
                            [&](auto& closestCurrVertex) 
                            {
                                Vector3 position = Vector3(closestCurrVertex.Position);
                                position         = Vector3::Transform(position, closestWorldMatrix);
                                bool intersects  = PointInOrientedBox(manipulateObbWorld, position);
                                if (true == intersects)
                                {
                                    closestVertexes.push_back(position);
                                }
                            });

                            manipulateVertexes.clear();
                            const Matrix& manipulateWorldMatrix = manipulateObject->transform->GetWorldMatrix();
                            char*         manipulateVertexBuff;
                            unsigned int  manipulateStride;
                            unsigned int  manipulateSize;
                            _manipulateBaseMesh->GetVertexInfo(manipulateVertexBuff, manipulateStride, manipulateSize);
                            // AABB culling
                            AutoVertexForeach(manipulateVertexBuff, manipulateStride, manipulateSize,
                            [&](auto& manipulateCurrVertex) 
                            {
                                Vector3 position = Vector3(manipulateCurrVertex.Position);
                                position        = Vector3::Transform(position, manipulateWorldMatrix);
                                bool intersects = PointInOrientedBox(manipulateObbWorld, position);
                                if (true == intersects)
                                {
                                    manipulateVertexes.push_back(position);
                                }
                            });

                            constexpr float maxFloat = std::numeric_limits<float>::max();
                            float closestVertexDis = maxFloat;
                            std::pair<Vector3, Vector3> closestVertex;
                            for (auto& currClosestVertexPos : closestVertexes)
                            {
                                for (auto& currManipulateVertexPos : manipulateVertexes)
                                {
                                    float currDis = Vector3::DistanceSquared(currClosestVertexPos, currManipulateVertexPos);
                                    if (currDis < closestVertexDis)
                                    {
                                        closestVertex.first  = currClosestVertexPos;
                                        closestVertex.second = currManipulateVertexPos;
                                        closestVertexDis     = currDis;
                                    }
                                }
                            }

                            if (closestVertexDis != maxFloat && closestVertexDis < ReflectFields->VertexSnapThreshold)
                            {
                                if (closestVertexDis > Mathf::Epsilon)
                                {
                                    Vector3 offset = closestVertex.first - closestVertex.second;
                                    snapPosition = manipulateObject->transform->Position + offset;
                                    manipulateObject->transform->Position = snapPosition;
                                    _isSnapping = true;
                                    mouseMoveDelta = 0.f;
                                }
                            }
                        }
                    }
                }
                else
                {
                    manipulateObject->transform->Position = snapPosition;

                    ImGuiIO& io = ImGui::GetIO();
                    float moveDelta = Vector2(&io.MouseDelta.x).Length();
                    mouseMoveDelta += moveDelta;
                    if (ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
                    {
                        _isSnapping = false;
                        mouseMoveDelta = 0.f;
                    }
                    if (mouseMoveDelta >= mouseMoveDeltaThresHold)
                    {
                        _isSnapping = false;
                        mouseMoveDelta = 0.f;
                    }
                }
            }
        }

    }
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
    Vector3 camPos = _camera->GetPivotPosition();
    std::memcpy(ReflectFields->CameraPosition.data(), &camPos, sizeof(ReflectFields->CameraPosition));

    Quaternion camRot = _camera->GetRotation();
    camRot.Normalize();
    std::memcpy(ReflectFields->CameraRotation.data(), &camRot, sizeof(ReflectFields->CameraRotation));

    UpdateReflectFields();
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
    _camera->SetPivot(ReflectFields->CameraPivot);
}

void EditorSceneTool::UpdateReflectFields() 
{
    ReflectFields->CameraMoveSpeed = _camera->GetMoveSpeed();
    ReflectFields->CameraRotateSpeed = _camera->GetRotationSpeed();
    ReflectFields->CameraPivot = _camera->GetPivot();
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

bool EditorSceneTool::ManipulateCommand::Execute() 
{
    if (false == _target.expired())
    {
        auto object = _target.lock();
        object->transform->Position = _curr.Position;
        object->transform->Rotation = _curr.Rotation;
        object->transform->Scale    = _curr.Scale;
        object->GetScene().IsDirty = true;
        return true;
    }
    return false;
}

void EditorSceneTool::ManipulateCommand::Undo() 
{
    if (false == _target.expired())
    {
        auto object = _target.lock();
        object->transform->Position = _prev.Position;
        object->transform->Rotation = _prev.Rotation;
        object->transform->Scale    = _prev.Scale;
        object->GetScene().IsDirty  = true;
    }
}
