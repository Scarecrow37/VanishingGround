#include "pch.h"
#include "Command/DetachChildrenCommand.h"
#include "Command/DropPrefabCommand.h"
#include "Command/PackPrefabCommand.h"
#include "Command/SetParentCommand.h"
#include "Editor/Tool/Scene/Command/EditorSceneCommands.h"
#include "Mesh/MeshComponent.h"
#include "Camera/CameraComponent.h"

using namespace u8_literals;
using namespace Global;
using namespace Command::Hierarchy;

static EditorSceneTool* staticEditorScenTool = nullptr;

void EditorHierarchyTool::TransformTreeNode(Transform& node, const std::shared_ptr<GameObject>& focusObject,
                                            GameObject*& outClickNode)
{
    EditorSceneTool* sceneTool = _editorSceneTool;
    auto TreeClickEvent = [&node, &outClickNode, &sceneTool]() {
        bool isHovered = ImGui::IsItemHovered();
        bool isDoubleClicked = ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
        bool isReleased      = ImGui::IsMouseReleased(ImGuiMouseButton_Left);

        bool result = false;
        if (isHovered)
        {
            if (isDoubleClicked)
            {
                if (sceneTool)
                {
                    sceneTool->SetCameraToObject(node.gameObject->GetWeakPtr());
                }
                result = true;
            }
            else if (isReleased)
            {
                outClickNode = &node.gameObject;
                result = true;
            }
        }
        return result;
    };

    auto TreeDragDropEvent = [&node]() {
        if (ImGui::BeginDragDropSource())
        {
            using Data                = DragDropTransform::Data;
            constexpr const char* key = DragDropTransform::KEY;

            Data data{};
            data.pTransform = &node;

            ImGui::SetDragDropPayload(key, &data, sizeof(Data));
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DragDropTransform::KEY))
            {
                DragDropTransform::Data* data         = (DragDropTransform::Data*)payload->Data;
                Transform*               prevParent   = data->pTransform->Parent;
                auto                     targetObject = data->pTransform->gameObject->GetWeakPtr();
                auto                     currObject   = node.gameObject->GetWeakPtr();
                if (nullptr != prevParent)
                {
                    auto prevObject = prevParent->gameObject->GetWeakPtr();
                    UmCommandManager.Do<Command::Hierarchy::SetParentCommand>(targetObject, prevObject, currObject);
                }
                else
                {
                    UmCommandManager.Do<Command::Hierarchy::SetParentCommand>(targetObject, nullptr, currObject);
                }
            }
            ImGui::EndDragDropTarget();
        }
    };

    auto TreeRightClickEvent = [&node]() {
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
            ImGui::OpenPopup("NodeContextMenu");

        if (ImGui::BeginPopup("NodeContextMenu"))
        {
            if (ImGui::MenuItem("Set Root Object"))
            {
                Transform* prevParent   = node.Parent;
                auto       targetObject = node.gameObject->GetWeakPtr();
                if (nullptr != prevParent)
                {
                    auto prevObject = prevParent->gameObject->GetWeakPtr();
                    UmCommandManager.Do<Command::Hierarchy::SetParentCommand>(targetObject, prevObject, nullptr);
                }
            }
            if (ImGui::MenuItem("Detach Children"))
            {
                UmCommandManager.Do<DetachChildrenCommand>(node.gameObject->GetWeakPtr());
            }
            if (ImGui::MenuItem("Destroy"))
            {
                node.gameObject->GetScene().IsDirty = true;
                UmCommandManager.Do<Command::EditorScene::DestroyGameObjectCommand>(&node.gameObject);
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("Prefab"))
            {
                if (ImGui::MenuItem("Unpack Prefab"))
                {
                    UmCommandManager.Do<PackPrefabCommand>(node.gameObject->GetWeakPtr(), "");
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (nullptr == node.Parent)
            {
                if (ImGui::MenuItem("Align With View"))
                {
                    Matrix     sceneCameraMatrix = staticEditorScenTool->GetCameraMatrix();
                    Vector3    pos;
                    Vector3    scale;
                    Quaternion rot;
                    sceneCameraMatrix.Decompose(scale, rot, pos);
                    node.Position = pos;
                    node.Rotation = rot;
                }
                ImGuiHelper::HoveredToolTip(
                    (const char*)u8"이 오브젝트의 위치와 회전을 Scene View의 값으로 설정합니다.");
            }
            ImGui::EndPopup();
        }
    };

    auto PushFocusStyle = [&node]() {
        if (node.gameObject->ActiveInHierarchy == false)
        {
            GameObject& object = node.gameObject;
            if (object.IsPrefabInstance() == false)
            {
                // 회색 계열
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
                return true;
            }
            else
            {
                std::string path = object.PrefabPath;
                if (path.empty() == false)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.25f, 0.5f, 1.0f)); 
                    return true;
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.4f, 0.4f, 1.0f));
                    return true;
                }
            }
        }
        else
        {
            GameObject& object = node.gameObject;
            if (object.IsPrefabInstance() == false)
            {
                // 기본 스타일 사용
                return false;
            }
            else
            {
                std::string path = object.PrefabPath;
                if (path.empty() == false)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.75f, 1.0f, 1.0f));
                    return true;
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
                    return true;
                }
            }
        }
        return false;
    };
    auto PopFocusStyle = [&node](bool isPushStyle) {
        if (isPushStyle)
        {
            ImGui::PopStyleColor();
        }
    };
    auto FocusRectDarw = [&node](GameObject* pFocusObject) {
        if (pFocusObject == &node.gameObject)
        {
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();

            ImVec2 windowPos  = ImGui::GetWindowPos();
            ImVec2 windowSize = ImGui::GetWindowSize();

            min.x = windowPos.x;
            max.x = windowPos.x + windowSize.x;

            constexpr float dampX = 3.f;
            min.x += dampX;
            max.x -= dampX;

            ImGui::GetWindowDrawList()->AddRect(min, max, IM_COL32(180, 180, 180, 255), 4.0f, 0, 1.5f);
        }
    };

    if (node.gameObject->IsValid())
    {
        ImGui::PushID(&node);
        bool isPushStyle = PushFocusStyle();
        if (static_isOpenFocusObj)
        {
            Transform* nodeRoot  = node.Root;
            nodeRoot             = nodeRoot ? nodeRoot : &node;
            Transform* focusRoot = nullptr;
            if (focusObject)
            {
                focusRoot = focusObject->transform->Root;
                focusRoot = focusRoot ? focusRoot : &focusObject->transform;
            }
            if (nodeRoot && focusRoot)
            {
                if (nodeRoot == focusRoot)
                {
                    if (&focusObject->transform != &node)
                    {
                        ImGui::SetNextItemOpen(true);
                    }
                    else
                    {
                        static_isOpenFocusObj = false;
                    }
                }
            }
        }

        GameObject&        gameObject = node.gameObject;
        ImGuiTreeNodeFlags treeFlags  = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (ImGui::TreeNodeEx(gameObject.ToString().data(), treeFlags))
        {
            FocusRectDarw(focusObject.get());
            PopFocusStyle(isPushStyle);
            TreeClickEvent();
            TreeRightClickEvent();
            TreeDragDropEvent();

            for (int i = 0; i < node.ChildCount; i++)
            {
                Transform* child = node.GetChild(i);
                if (child)
                {
                    TransformTreeNode(*child, focusObject, outClickNode);
                }
            }
            ImGui::TreePop();
        }
        else
        {
            FocusRectDarw(focusObject.get());
            PopFocusStyle(isPushStyle);
            TreeClickEvent();
            TreeRightClickEvent();
            TreeDragDropEvent();
        }
        ImGui::PopID();
    }
}

void EditorHierarchyTool::SetFocusObject(const std::weak_ptr<GameObject>& object)
{
    if (false == static_hierarchyFocusObjWeak.expired())
    {
        auto prevFocus = static_hierarchyFocusObjWeak.lock();
        for (int i = 0; i < prevFocus->GetComponentCount(); ++i)
        {
            Component* component = prevFocus->GetComponentAtIndex<Component>(i);
            if (component->GetType() == Component::TYPE::MESH)
            {
                MeshComponent* mesh = static_cast<MeshComponent*>(component);
                if (mesh->Renderer)
                {
                    mesh->Renderer->OffCustomDepth(PostProcess::OUTLINE);
                }
            }
        }
    }

    static_hierarchyFocusObjWeak = object;

    if (false == static_hierarchyFocusObjWeak.expired())
    {
        auto focus = static_hierarchyFocusObjWeak.lock();
        for (int i = 0; i < focus->GetComponentCount(); ++i)
        {
            Component* component = focus->GetComponentAtIndex<Component>(i);
            if (component->GetType() == Component::TYPE::MESH)
            {
                MeshComponent* mesh = static_cast<MeshComponent*>(component);
                if (mesh->Renderer)
                {
                    mesh->Renderer->OnCustomDepth(PostProcess::OUTLINE);
                }
            }
        }
        static_isOpenFocusObj = true;
    }
}

bool EditorHierarchyTool::SaveScene(Scene& scene)
{
    std::filesystem::path writePath = (std::string)scene.Path;
    writePath                       = std::filesystem::relative(writePath, UmFileSystem.GetAssetPath()).parent_path();
    UmSceneManager.WriteSceneToFile(scene, writePath.string(), true);
    return true;
}

EditorHierarchyTool::EditorHierarchyTool()
{
    SetLabel("Hierarchy");
    SetDockLayout(ImGuiDir_Left);
}

EditorHierarchyTool::~EditorHierarchyTool() {}

void EditorHierarchyTool::ImGuiNewGameObjectMenuItems()
{
    static const char* GameObjectKey = typeid(GameObject).name();
    if (ImGui::BeginMenu("Object"))
    {
        static const char* GameObjectName = GameObjectKey + 6;
        if (ImGui::MenuItem(GameObjectName))
        {
            UmCommandManager.Do<Command::EditorScene::NewGameObjectCommand>(
                GameObjectKey, GameObject::Helper::GenerateUniqueName(GameObjectName));
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Camera"))
    {
        GameObject* camera = nullptr;
        if (ImGui::MenuItem("Main camera"))
        {
            UmCommandManager.Do<Command::EditorScene::NewGameObjectCommand>(
                GameObjectKey, GameObject::Helper::GenerateUniqueName("Main camera"), &camera);

            if (camera)
            {             
                CameraComponent& cameraComponent = camera->AddComponent<CameraComponent>();
                cameraComponent.SetMainCamera();
            }
        }
        if (ImGui::MenuItem("Camera"))
        {
            UmCommandManager.Do<Command::EditorScene::NewGameObjectCommand>(
                GameObjectKey, GameObject::Helper::GenerateUniqueName("Camera"), &camera);

            if (camera)
            {
                camera->AddComponent<CameraComponent>();
            }
        }
        ImGui::EndMenu();

        if (camera != nullptr)
        {
            camera->AddTag("Camera");
        }
    }

    if (ImGui::BeginMenu("Light"))
    {
        GameObject* light = nullptr;
        if (ImGui::MenuItem("Directional light"))
        {
            UmCommandManager.Do<Command::EditorScene::NewGameObjectCommand>(
                GameObjectKey, GameObject::Helper::GenerateUniqueName("Directional light"), &light);
            UmComponentFactory.AddComponentToObject(light, "class DirectionalLight");
        }
        if (ImGui::MenuItem("Point light"))
        {
            UmCommandManager.Do<Command::EditorScene::NewGameObjectCommand>(
                GameObjectKey, GameObject::Helper::GenerateUniqueName("Point light"), &light);
            UmComponentFactory.AddComponentToObject(light, "class PointLight");
        }
        if (ImGui::MenuItem("Spot light"))
        {
            UmCommandManager.Do<Command::EditorScene::NewGameObjectCommand>(
                GameObjectKey, GameObject::Helper::GenerateUniqueName("Spot light"), &light);
            UmComponentFactory.AddComponentToObject(light, "class SpotLight");
        }
        ImGui::EndMenu();

        if (nullptr != light)
        {
            light->AddTag("Light");
        }
    }

    if (ImGui::BeginMenu("Mesh"))
    {
        GameObject* mesh = nullptr;
        if (ImGui::MenuItem("Static Mesh"))
        {
            UmCommandManager.Do<Command::EditorScene::NewGameObjectCommand>(
                GameObjectKey, GameObject::Helper::GenerateUniqueName("Static Mesh"), &mesh);
            UmComponentFactory.AddComponentToObject(mesh, "class StaticMeshRenderer");
        }
        if (ImGui::MenuItem("Skeletal Mesh"))
        {
            UmCommandManager.Do<Command::EditorScene::NewGameObjectCommand>(
                GameObjectKey, GameObject::Helper::GenerateUniqueName("Skeletal Mesh"), &mesh);
            UmComponentFactory.AddComponentToObject(mesh, "class SkeletalMeshRenderer");
            UmComponentFactory.AddComponentToObject(mesh, "class AnimationComponent");
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("UI"))
    {
        GameObject* ui = nullptr;
        if (ImGui::MenuItem("UI Root"))
        {
            UmCommandManager.Do<Command::EditorScene::NewGameObjectCommand>(
                GameObjectKey, GameObject::Helper::GenerateUniqueName("UI Root"), &ui);
            UmComponentFactory.AddComponentToObject(ui, "class UIRoot");
        }
        if (ImGui::BeginMenu("Panels"))
        {
            if (ImGui::MenuItem("Grid Panel"))
            {
                UmCommandManager.Do<Command::EditorScene::NewGameObjectCommand>(
                    GameObjectKey, GameObject::Helper::GenerateUniqueName("Grid Panel"), &ui);
                UmComponentFactory.AddComponentToObject(ui, "class GridPanel");
            }
            if (ImGui::MenuItem("Horizontal Panel"))
            {
                UmCommandManager.Do<Command::EditorScene::NewGameObjectCommand>(
                    GameObjectKey, GameObject::Helper::GenerateUniqueName("Horizontal Panel"), &ui);
                UmComponentFactory.AddComponentToObject(ui, "class HorizontalPanel");
            }
            if (ImGui::MenuItem("Description Panel"))
            {
                UmCommandManager.Do<Command::EditorScene::NewGameObjectCommand>(
                    GameObjectKey, GameObject::Helper::GenerateUniqueName("Description Panel"), &ui);
                UmComponentFactory.AddComponentToObject(ui, "class DescriptionPanel");
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Wrappers"))
        {
            if (ImGui::MenuItem("Dummy Wrapper"))
            {
                UmCommandManager.Do<Command::EditorScene::NewGameObjectCommand>(
                    GameObjectKey, GameObject::Helper::GenerateUniqueName("Dummy Wrapper"), &ui);
                UmComponentFactory.AddComponentToObject(ui, "class DummyWrapper");
            }
            if (ImGui::MenuItem("Ratio Wrapper"))
            {
                UmCommandManager.Do<Command::EditorScene::NewGameObjectCommand>(
                    GameObjectKey, GameObject::Helper::GenerateUniqueName("Ratio Wrapper"), &ui);
                UmComponentFactory.AddComponentToObject(ui, "class RatioWrapper");
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Elements"))
        {
            if (ImGui::MenuItem("Dummy Element"))
            {
                UmCommandManager.Do<Command::EditorScene::NewGameObjectCommand>(
                    GameObjectKey, GameObject::Helper::GenerateUniqueName("Dummy Element"), &ui);
                UmComponentFactory.AddComponentToObject(ui, "class DummyElement");
            }
            if (ImGui::MenuItem("Image Element"))
            {
                UmCommandManager.Do<Command::EditorScene::NewGameObjectCommand>(
                    GameObjectKey, GameObject::Helper::GenerateUniqueName("Image Element"), &ui);
                UmComponentFactory.AddComponentToObject(ui, "class ImageElement");
            }
            if (ImGui::MenuItem("Text Element"))
            {
                UmCommandManager.Do<Command::EditorScene::NewGameObjectCommand>(
                    GameObjectKey, GameObject::Helper::GenerateUniqueName("Text Element"), &ui);
                UmComponentFactory.AddComponentToObject(ui, "class TextElement");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
        if (ui != nullptr)
            ui->AddTag("UI");
    }
}

void EditorHierarchyTool::OnStartGui()
{
    _dockWindow          = GetOwnerDockWindow();
    _editorSceneTool     = _dockWindow->GetGui<EditorSceneTool>();
    staticEditorScenTool = _editorSceneTool;
    _editorFindTool      = _dockWindow->GetGui<HierarchyFindTool>();
}

void EditorHierarchyTool::OnPreFrameBegin() {}

void EditorHierarchyTool::OnPostFrameBegin() {}

void EditorHierarchyTool::HierarchyDropEvent()
{
    namespace fs = std::filesystem;
    ImRect rect  = _window->Rect();
    if (ImGui::BeginDragDropTargetCustom(rect, _window->ID))
    {
        // 에셋에 대한 드래그 앤 드롭 이벤트 처리
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
        {
            DragDropAsset::Data* data = (DragDropAsset::Data*)payload->Data;
            const File::Path&    path = data->GetPath();
            fs::path extension = path.extension();
            if (extension == UmGameObjectFactory.PREFAB_EXTENSION)
            {
                UmCommandManager.Do<DropPrefabCommand>(path.ToGuid());
            }
            else if (extension == UmSceneManager.SCENE_EXTENSION)
            {
                UmSceneManager.LoadScene(path.string(), LoadSceneMode::ADDITIVE);
            }
        }
        // Transform에 대한 드래그 앤 드롭 이벤트 처리
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DragDropTransform::KEY))
        {
            DragDropTransform::Data* data = (DragDropTransform::Data*)payload->Data;
            // 데이터 Null 확인
            if (data && data->pTransform)
            {
                // 1. Hierarchy 프레임에 드롭한 것은 Root로 설정하겠다는 것
                Transform* current = data->pTransform;
                Transform* parent  = data->pTransform->Parent;
                // 때문에 부모가 없으면 커맨드를 실행할 필요가 없음.
                if (current && parent)
                {
                    std::weak_ptr<GameObject> currentWeak, parentWeak;
                    currentWeak = current->gameObject->GetWeakPtr();
                    parentWeak  = parent->gameObject->GetWeakPtr();
                    UmCommandManager.Do<Command::Hierarchy::SetParentCommand>(currentWeak, parentWeak, nullptr);
                }
                
            }
        }
        ImGui::EndDragDropTarget();
    }
}

void EditorHierarchyTool::HierarchyRightClickEvent() const
{
    if (ImGui::BeginPopupContextWindow("HierarchyRightClickPopup", ImGuiPopupFlags_NoOpenOverItems |
                                                                       ImGuiPopupFlags_MouseButtonRight |
                                                                       ImGuiPopupFlags_NoOpenOverExistingPopup))
    {
        ImGui::Text("New GameObject");
        ImGui::Separator();
        ImGuiNewGameObjectMenuItems();
        ImGui::EndPopup();
    }
}

void EditorHierarchyTool::KeyboardEvent()
{
    if (Global::editorModule->IsFocusAreaEmpty())
    {
        if (_dockWindow->IsFocusFrame())
        {
            bool holdCtrl = ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl);
            if (holdCtrl)
            {
                if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S, false))
                {
                    Scene* scene = UmSceneManager.GetMainScene();
                    if (scene)
                    {
                        std::filesystem::path writePath = (std::string)scene->Path;
                        writePath = std::filesystem::relative(writePath, UmFileSystem.GetAssetPath()).parent_path();
                        UmSceneManager.WriteSceneToFile(*scene, writePath.string(), true);
                    }
                }
            }

            if (this->IsFocusFrame() || _editorSceneTool->IsFocusFrame() || _editorFindTool->IsFocusFrame())
            {
                if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete, false))
                {
                    if (false == static_hierarchyFocusObjWeak.expired())
                    {
                        auto object                = static_hierarchyFocusObjWeak.lock();
                        object->GetScene().IsDirty = true;
                        UmCommandManager.Do<Command::EditorScene::DestroyGameObjectCommand>(object.get());
                    }
                }
            }
        }

        if (this->IsFocusFrame())
        {
            bool holdCtrl = ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl);
            if (holdCtrl)
            {
                if (ImGui::IsKeyPressed(ImGuiKey_C, false))
                {
                    const auto& weakObject = GetFocusObject();
                    if (auto object = weakObject.lock())
                    {
                        YAML::Node yamlNode = UmGameObjectFactory.SerializeToYaml(object.get(), true);
                        YAML::Emitter emitter;
                        emitter << yamlNode;
                        if (emitter.good())
                        {
                            std::wstring yamlDataWstring = U8ToWString(emitter.c_str());
                            File::SetClipboardText(yamlDataWstring);
                        }
                    }         
                }
                if (ImGui::IsKeyPressed(ImGuiKey_V, false))
                {
                    std::wstring clipboardText = File::GetClipboardText();
                    if (false == clipboardText.empty())
                    {
                        UmCommandManager.Do<Command::EditorScene::PasteObjectCommand>(clipboardText);
                    }
                }
            }

        }
    }
}

void EditorHierarchyTool::SerializedReflectEvent()
{
    const auto& scenes = engineCore->SceneManager.GetLoadedScenes();
    for (auto& scene : scenes)
    {
        SaveScene(*scene);
    }
}

void EditorHierarchyTool::DeserializedReflectEvent() {}

void EditorHierarchyTool::OnFrameRender()
{
    std::shared_ptr<GameObject> focusObject = static_hierarchyFocusObjWeak.lock();
    _window                                 = ImGui::GetCurrentWindow();
    HierarchyRightClickEvent();
    HierarchyDropEvent();
    KeyboardEvent();

    ImVec2 size = ImGui::GetContentRegionAvail();
    size.y -= 25;
    ImGui::BeginChild("##E8DA04FA-E996-4718-8E2F-3138772C5A32", size);
    HierarchyRightClickEvent();
    const auto& scenes = engineCore->SceneManager.GetLoadedScenes();
    if (false == scenes.empty())
    {
        for (auto& pScene : scenes)
        {
            ImGui::PushID(pScene);
            {
                Scene& scene = *pScene;
                if (scene.isLoaded == false)
                    continue;

                std::string sName = scene.Name;
                bool        isCollapsingOpen =
                    ImGui::CollapsingHeader(sName.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen);
                if (ImGui::BeginPopupContextItem("RightClick"))
                {
                    if (true == _isPlay)
                    {
                        ImGui::BeginDisabled();
                    }

                    if (ImGui::MenuItem("Save Scene"))
                    {
                        SaveScene(scene);
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::MenuItem("Unload Scene"))
                    {
                        std::string path = scene.Path;
                        UmSceneManager.UnloadScene(path);
                        ImGui::CloseCurrentPopup();
                    }

                    if (true == _isPlay)
                    {
                        ImGui::EndDisabled();
                    }
                    ImGui::EndPopup();
                }
                if (true == scene.IsDirty)
                {
                    ImGui::SameLine();
                    ImGui::Text("*");
                }
                if (isCollapsingOpen)
                {
                    auto rootObjects = scene.GetRootGameObjects();
                    for (auto& obj : rootObjects)
                    {
                        ImGui::PushID(obj.get());
                        {
                            GameObject* clickNode = nullptr;
                            TransformTreeNode(obj->transform, focusObject, clickNode);
                            if (clickNode)
                            {
                                auto& oldWp = EditorHierarchyTool::static_hierarchyFocusObjWeak;
                                auto  newWp = clickNode->GetWeakPtr();
                                if (false == EditorInspectorTool::IsLockFocus() &&
                                    false == EditorInspectorTool::IsFocusObject(newWp))
                                {
                                    UmCommandManager.Do<Command::Hierarchy::FocusCommand>(oldWp, newWp);
                                }
                            }
                        }
                        ImGui::PopID();
                    }
                }
            }
            ImGui::PopID();
        }
        static_isOpenFocusObj = false;
    }

    if (_isPlay)
    {
        Scene* pDontDestroyOnLoad = UmSceneManager.GetDontDestroyOnLoadScene();
        if (nullptr != pDontDestroyOnLoad)
        {
            bool isCollapsingOpen =
                ImGui::CollapsingHeader("DontDestroyOnLoad", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen);
            if (isCollapsingOpen)
            {
                auto rootObjects = pDontDestroyOnLoad->GetRootGameObjects();
                for (auto& obj : rootObjects)
                {
                    ImGui::PushID(obj.get());
                    {
                        GameObject* clickNode = nullptr;
                        TransformTreeNode(obj->transform, focusObject, clickNode);
                        if (clickNode)
                        {
                            auto& oldWp = EditorHierarchyTool::static_hierarchyFocusObjWeak;
                            auto  newWp = clickNode->GetWeakPtr();
                            if (false == EditorInspectorTool::IsLockFocus() &&
                                false == EditorInspectorTool::IsFocusObject(newWp))
                            {
                                UmCommandManager.Do<Command::Hierarchy::FocusCommand>(oldWp, newWp);
                            }
                        }
                    }
                    ImGui::PopID();
                }
            }
        }
    }

    ImGui::EndChild();
}

void EditorHierarchyTool::OnFrameEnd() 
{
    
}

void EditorHierarchyTool::OnFramePopupOpened() {}

void EditorHierarchyTool::OnTickGui()
{
    _isPlay = editorModule->PlayMode.IsPlay();
}

void EditorHierarchyTool::OnFrameFocusStay() {}
