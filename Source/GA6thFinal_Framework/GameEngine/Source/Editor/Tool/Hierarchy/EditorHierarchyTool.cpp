#include "pch.h"
#include "EditorHierarchyTool.h"

using namespace u8_literals;
using namespace Global;

static std::weak_ptr<GameObject> HierarchyFocusObjWeak;
static void TransformTreeNode(Transform& node, const std::shared_ptr<GameObject>& focusObject)
{
    auto TreeDoubleClickEvent = [&node]() {
        bool result = ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered();
        if (result)
        {
            HierarchyFocusObjWeak = node.gameObject->GetWeakPtr();
            EditorInspectorTool::SetFocusObject(HierarchyFocusObjWeak);
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

            ImGui::SetDragDropPayload(key, 
                                      &data, 
                                      sizeof(Data));
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = 
                ImGui::AcceptDragDropPayload(DragDropTransform::KEY))
            {
                DragDropTransform::Data* data = (DragDropTransform::Data*)payload->Data;
                data->pTransform->SetParent(node);
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
                node.SetParent(nullptr);
            }
            if (ImGui::MenuItem("Detach Children"))
            {
                node.DetachChildren();
            }
            if (ImGui::MenuItem("Destroy"))
            {
                GameObject::Destroy(&node.gameObject);
            }
            ImGui::EndPopup();
        }
    };

    auto PushFocusStyle = [&node](GameObject* pFocusObject) {
        if (pFocusObject)
        {
            Transform* curr = &pFocusObject->transform;

            if (curr == &node)
            {
                ImGui::PushStyleColor(
                    ImGuiCol_Text, ImVec4(0.4f, 0.75f, 1.0f, 1.0f)); // 글자색
                ImGui::PushStyleColor(
                    ImGuiCol_HeaderHovered,
                    ImVec4(0.2f, 0.45f, 0.8f, 1.0f)); // 포커스시
                ImGui::PushStyleColor(
                    ImGuiCol_HeaderActive,
                    ImVec4(0.25f, 0.55f, 0.9f, 1.0f)); // 클릭시
                return true;
            }
        }
        return false;
    };

    auto PopFocusStyle = [&node](GameObject* pFocusObject) {
        if (pFocusObject)
        {
            Transform* curr = &pFocusObject->transform;
            if (curr == &node)
            {
                ImGui::PopStyleColor(3);

                return true;
            }
        }
        return false;
    };

    ImGui::PushID(&node);
    PushFocusStyle(focusObject.get());
    if (ImGui::TreeNodeEx(node.gameObject->ToString().data(),
                          ImGuiTreeNodeFlags_OpenOnArrow))
    {
        PopFocusStyle(focusObject.get());
        TreeDoubleClickEvent();
        TreeRightClickEvent();
        TreeDragDropEvent();

        for (int i = 0; i < node.ChildCount; i++)
        {
            Transform* child = node.GetChild(i);
            if (child)
            {
                TransformTreeNode(*child, focusObject);
            }
        }
        ImGui::TreePop();
    }
    else
    {
        PopFocusStyle(focusObject.get());
        TreeDoubleClickEvent();
        TreeRightClickEvent();
        TreeDragDropEvent();
    }
    ImGui::PopID();
}


EditorHierarchyTool::EditorHierarchyTool()
{
    SetLabel("Hierarchy");
    SetDockLayout(DockLayout::LEFT);
}

EditorHierarchyTool::~EditorHierarchyTool()
{
}

void  EditorHierarchyTool::OnStartGui()
{
   
}

void  EditorHierarchyTool::OnPreFrame()
{
    
}

void EditorHierarchyTool::HierarchyDropEvent()
{
    namespace fs = std::filesystem;
    ImGuiWindow* window = ImGui::FindWindowByName(GetLabel().c_str());
    ImRect       rect   = window->Rect();
    if (ImGui::BeginDragDropTargetCustom(rect, window->ID))
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
        {
            DragDropAsset::Data* data = (DragDropAsset::Data*)payload->Data;      
            if (data->context->expired() == false)
            {
                auto context = data->context->lock();
                const File::Path& path = context->GetPath();
                fs::path extension = path.extension();
                if (extension == UmGameObjectFactory.PREFAB_EXTENSION)
                {
                    UmGameObjectFactory.DeserializeToGuid(path.ToGuid());
                }
                else if (extension == UmSceneManager.SCENE_EXTENSION)
                {
                    UmSceneManager.LoadScene(path.string(), LoadSceneMode::ADDITIVE);
                }        
            }
        }
        ImGui::EndDragDropTarget();
    }
}

void  EditorHierarchyTool::OnFrame()
{
    HierarchyDropEvent();
    const auto& scenes = engineCore->SceneManager.GetLoadedScenes();
    for (auto& pScene : scenes)
    {
        ImGui::PushID(pScene);
        {
            Scene& scene = *pScene;
            if (scene.isLoaded == false)
                continue;

            std::string sName = scene.Name;
            if (ImGui::CollapsingHeader(sName.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::BeginPopupContextItem("RightClick"))
                {
                    if (ImGui::Button("Save Scene"))
                    {
                        std::string           path = scene.Path;
                        std::filesystem::path writePath =
                            std::filesystem::relative(path, UmFileSystem.GetRootPath()).parent_path();
                        UmSceneManager.WriteSceneToFile(scene, writePath.string());
                    }
                    if (ImGui::Button("Unload Scene"))
                    {
                        std::string path = scene.Path;
                        UmSceneManager.UnloadScene(path);
                    }
                    ImGui::EndPopup();
                }

                auto rootObjects = scene.GetRootGameObjects();
                std::shared_ptr<GameObject> focusObject = HierarchyFocusObjWeak.lock();
                for (auto& obj : rootObjects)
                {
                    ImGui::PushID(obj.get());
                    {
                        TransformTreeNode(obj->transform, focusObject);
                    }
                    ImGui::PopID();
                }
            }
        }      
        ImGui::PopID();
    }
}

void  EditorHierarchyTool::OnPostFrame()
{
    
}

void EditorHierarchyTool::OnPopup()
{
  
}
