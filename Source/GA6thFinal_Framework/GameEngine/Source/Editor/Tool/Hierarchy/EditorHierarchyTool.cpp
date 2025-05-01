#include "pch.h"
#include "EditorHierarchyTool.h"

using namespace u8_literals;
using namespace Global;

static std::weak_ptr<GameObject> HierarchyFocusObjWeak;
static void TransformTreeNode(Transform& node, const std::shared_ptr<GameObject>& focusObject)
{
    auto TreeDoubleClickEvent = [&node]() {
        bool result = ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::IsItemHovered();
        if (result)
        {
            //HierarchyFocusObjWeak = node.gameObject->GetWeakPtr();
            //EditorInspectorTool::SetFocusObject(HierarchyFocusObjWeak);

            auto oldWp = HierarchyFocusObjWeak;
            auto newWp = node.gameObject->GetWeakPtr();
            if (false == EditorInspectorTool::IsFocused(newWp))
            {
                UmCommandManager.Do<Command::FocusInspecor>(oldWp, newWp);
                HierarchyFocusObjWeak = newWp;
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

    auto PushFocusStyle = [&node]() 
    {
        if (node.gameObject->ActiveInHierarchy == false)
        {
            GameObject& object  = node.gameObject;
            const type_info& type_id = typeid(object);
            if (typeid(GameObject) == type_id)
            {     
                //회색 계열
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));         
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f)); 
                ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));  
                return true;
            }
        }
        else
        {
            GameObject& object  = node.gameObject;
            const type_info& type_id = typeid(object);
            if (typeid(GameObject) == type_id)
            {
                //기본 스타일 사용
                return false;
            }
        }
        return false;
    };
    auto PopFocusStyle = [&node](bool isPushStyle) 
    {
        if (isPushStyle)
        {
            ImGui::PopStyleColor(3);
        }
    };
    auto FocusRectDarw = [&node](GameObject* pFocusObject) 
    {
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

    ImGui::PushID(&node);
    bool isPushStyle = PushFocusStyle();
    if (ImGui::TreeNodeEx(node.gameObject->ToString().data(),
                          ImGuiTreeNodeFlags_OpenOnArrow))
    {
        FocusRectDarw(focusObject.get());
        PopFocusStyle(isPushStyle);
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
        FocusRectDarw(focusObject.get());
        PopFocusStyle(isPushStyle);
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
    ImRect rect = window->Rect();
    if (ImGui::BeginDragDropTargetCustom(rect, window->ID))
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
        {
            DragDropAsset::Data* data = (DragDropAsset::Data*)payload->Data;      
            std::weak_ptr<File::Context>* wpContext = data->pContext;
            if (false == wpContext->expired())
            {
                auto context = wpContext->lock();
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

void EditorHierarchyTool::HierarchyRightClickEvent() const 
{
    if (ImGui::BeginPopupContextWindow("HierarchyRightClickPopup",
        ImGuiPopupFlags_NoOpenOverItems |
        ImGuiPopupFlags_MouseButtonRight |
        ImGuiPopupFlags_NoOpenOverExistingPopup)
       )
    {
        ImGui::Text("New GameObject");
        ImGui::Separator();
        EditorSceneMenuGameObject::ImGuiNewGameObjectMenuItems();
        ImGui::EndPopup();
    }
}

void EditorHierarchyTool::OnFrame()
{
    window = ImGui::GetCurrentWindow();
    HierarchyRightClickEvent();
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
                    if (ImGui::MenuItem("Save Scene"))
                    {
                        std::string           path = scene.Path;
                        std::filesystem::path writePath =
                            std::filesystem::relative(path, UmFileSystem.GetRootPath()).parent_path();
                        UmSceneManager.WriteSceneToFile(scene, writePath.string(), true);
                        ImGui::CloseCurrentPopup();
                    }
                    if (ImGui::MenuItem("Unload Scene"))
                    {
                        std::string path = scene.Path;
                        UmSceneManager.UnloadScene(path);
                        ImGui::CloseCurrentPopup();
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

