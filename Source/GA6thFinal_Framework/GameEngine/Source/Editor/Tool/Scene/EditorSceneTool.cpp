#include "pch.h"
#include "Editor/Tool/Hierarchy/EditorHierarchyTool.h"
#include "Editor/Tool/Hierarchy/Command/FocusCommand.h"
#include "Editor/Tool/Scene/Command/EditorSceneCommands.h"
#include "Editor/DynamicCamera/EditorDynamicCamera.h"
#include "EditorSceneTool.h"
#include "UmScripts.h"
#include "Engine/GraphicsCore/Model.h"
#include "Engine/GraphicsCore/BaseMesh.h"

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

const Matrix& EditorSceneTool::GetCameraMatrix()
{
    return _camera->GetCamera()->GetWorldMatrix();
}

void EditorSceneTool::OnStartGui()
{
    std::shared_ptr<Camera> camera = UmRenderer.GetCamera("Editor");
    GRAPHICS_ASSERT(nullptr != camera, L"Camera is nullptr");

    _camera->SetTarget(camera);
    _dockWindow = GetOwnerDockWindow();
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
    if (ImGui::IsWindowHovered())
    {
        if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_MouseRight, false))
        {
            ImGui::SetWindowFocus();
        }
    }
    DragDropEvent();
    
    SetCamera();    
    DrawSceneView();
    DrawManipulate();
    RayPicker();
    VertexSnap();
}

void EditorSceneTool::OnFrameEnd()
{
}

void EditorSceneTool::OnFrameFocusEnter() 
{
}

void EditorSceneTool::OnFrameFocusStay()
{
    _camera->Update();

    UpdateKeyboardShortcuts();
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

void EditorSceneTool::UpdateKeyboardShortcuts()
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

        if (ImGui::IsKeyPressed(ImGuiKey_F, false))
        {
            SetCameraToFocusObject();
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

            _drawManipulateDesc.ViewDesc.ClientRight = _sceneClientRight;
            _drawManipulateDesc.ViewDesc.ClientTop   = _sceneClientTop;

            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(_sceneClienttLeft, _sceneClientTop, _sceneClientWidth, _sceneClientHeight);
            _isUseManipulate = ImGuiHelper::DrawManipulate(pDynamicCamera, pObjectMatrix, _drawManipulateDesc);
            _isUsing         = ImGuizmo::IsUsing();
            _isOver          = ImGuizmo::IsOver();

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
    ImGui::Image((ImTextureID)handle.ptr, {_sceneClientWidth, _sceneClientHeight});  

    constexpr ImVec2 iconButtonSize = ImVec2(64.0f, 64.0f);
    constexpr ImVec2 damp = ImVec2(4.f, 4.f);
    ImVec2 moveIconPos = _window->ContentRegionRect.Min;
    ImGui::SetCursorScreenPos(ImVec2(moveIconPos.x + damp.x, moveIconPos.y + damp.y));
    
    static std::shared_ptr<Texture> moveIconTexture = UmResourceManager.LoadResource<Texture>(L"../GameEngine/Icon/Editor/Move.png");
    static D3D12_GPU_DESCRIPTOR_HANDLE moveIconHandle = moveIconTexture->GetGPUHandle();

    static std::shared_ptr<Texture> rotationIconTexture = UmResourceManager.LoadResource<Texture>(L"../GameEngine/Icon/Editor/Rotate.png");
    static D3D12_GPU_DESCRIPTOR_HANDLE rotationIconHandle = rotationIconTexture->GetGPUHandle();

    static std::shared_ptr<Texture> scaleIconTexture = UmResourceManager.LoadResource<Texture>(L"../GameEngine/Icon/Editor/Scale.png");
    static D3D12_GPU_DESCRIPTOR_HANDLE scaleIconHandle = scaleIconTexture->GetGPUHandle();

    static std::shared_ptr<Texture> transformIconTexture = UmResourceManager.LoadResource<Texture>(L"../GameEngine/Icon/Editor/Transform.png");
    static D3D12_GPU_DESCRIPTOR_HANDLE transformIconHandle = transformIconTexture->GetGPUHandle();

    static std::shared_ptr<Texture> worldIconTexture = UmResourceManager.LoadResource<Texture>(L"../GameEngine/Icon/Editor/World.png");
    static D3D12_GPU_DESCRIPTOR_HANDLE worldIconHandle = worldIconTexture->GetGPUHandle();

    static std::shared_ptr<Texture> localIconTexture = UmResourceManager.LoadResource<Texture>(L"../GameEngine/Icon/Editor/Local.png");
    static D3D12_GPU_DESCRIPTOR_HANDLE localIconHandle = localIconTexture->GetGPUHandle();

    static std::shared_ptr<Texture> gridSnapIconTexture = UmResourceManager.LoadResource<Texture>(L"../GameEngine/Icon/Editor/GridSnap.png");
    static D3D12_GPU_DESCRIPTOR_HANDLE gridSnapIconHandle = gridSnapIconTexture->GetGPUHandle();

    static std::shared_ptr<Texture> toggleLeftTexture = UmResourceManager.LoadResource<Texture>(L"../GameEngine/Icon/Editor/ToggleLeft.png");
    static D3D12_GPU_DESCRIPTOR_HANDLE toggleLeftHandle = toggleLeftTexture->GetGPUHandle();

    static std::shared_ptr<Texture> toggleRightTexture = UmResourceManager.LoadResource<Texture>(L"../GameEngine/Icon/Editor/ToggleRight.png");
    static D3D12_GPU_DESCRIPTOR_HANDLE toggleRightHandle = toggleRightTexture->GetGPUHandle();

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

        const auto& runtimeObjects = ESceneManager::Engine::GetRuntimeObjects();
        for (auto& object : runtimeObjects)
        {
            if (object && object->IsValid())
            {
                for (size_t i = 0; i < object->GetComponentCount(); ++i)
                {
                    Component* component = object->GetComponentAtIndex<Component>(i);
                    if (component)
                    {
                        component->OnDrawDebug();
                    }
                }
            }
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
}

void EditorSceneTool::SetCameraToFocusObject() 
{
    if (false == EditorHierarchyTool::GetFocusObject().expired())
    {
        auto focusObject = EditorHierarchyTool::GetFocusObject().lock();
        _camera->SetPosition(focusObject->transform->Position);
    }
}

void EditorSceneTool::RayPicker() 
{
    if (false == _isOver && 
        false == _isUsingStart && 
        false == _isUsingEnd)
    {
        if (IsFocusFrame() && ImGui::IsWindowHovered() && ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
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
                        meshComponents.push_back(mesh);
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
                        auto& meshes = meshComponent->Renderer->GetModel()->GetMeshes();
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
static void AutoVertexForeach(char* vertexBuff, unsigned int stride, unsigned int size, Func func)
{
    if (stride == sizeof(StaticMeshVertex))
    {
        StaticMeshVertex* vertexes = reinterpret_cast<StaticMeshVertex*>(vertexBuff);
        for (size_t i = 0; i < size; ++i)
        {
            StaticMeshVertex& vertex = vertexes[i];
            func(vertex);
        }
    }
    else if (stride == sizeof(SkeletalMeshVertex))
    {
        SkeletalMeshVertex* vertexes = reinterpret_cast<SkeletalMeshVertex*>(vertexBuff);
        for (size_t i = 0; i < size; ++i)
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
                            const auto& manipulateMeshes = manipulateMesh->Renderer->GetModel()->GetMeshes();
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
                                        if (false == meshComponent->Enable || false == meshComponent->gameObject->ActiveInHierarchy)
                                        {
                                            continue;
                                        }

                                        if (nullptr != meshComponent->Renderer)
                                        {
                                            const Matrix& meshMatrix = meshComponent->gameObject->transform->GetWorldMatrix();
                                            BoundingOrientedBox meshObbWorld;
                                            const auto& meshes = meshComponent->Renderer->GetModel()->GetMeshes();
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

                                        if (true == intersects)
                                            break;
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
                            const auto& meshes = meshComponent->Renderer->GetModel()->GetMeshes();
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

void EditorSceneTool::ManipulateCommand::Execute() 
{
    if (false == _target.expired())
    {
        auto object = _target.lock();
        object->transform->Position = _curr.Position;
        object->transform->Rotation = _curr.Rotation;
        object->transform->Scale    = _curr.Scale;
        object->GetScene().IsDirty = true;
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
        object->GetScene().IsDirty  = true;
    }
}
