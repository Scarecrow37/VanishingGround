#include "ObjectTestEditor.h"
#include "Source/EditorTools/EditorInspectorView.h"

using namespace u8_literals;
using namespace Global;

static std::weak_ptr<GameObject> HierarchyFocusObjWeak;
static void TransformTreeNode(Transform& node, const std::shared_ptr<GameObject>& focusObject)
{
    auto TreeDoubleClickEvent = [&node]() 
    {
        bool result = ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered();
        if (result)
        {
            HierarchyFocusObjWeak = node.gameObject.GetWeakPtr();
            EditorInspectorView::SetFocusObject(HierarchyFocusObjWeak);
        }
        return result;
    };

    auto TreeDragDropEvent = [&node]() 
    {
        static Transform* pDragNode = nullptr;
        if (ImGui::BeginDragDropSource())
        {
            pDragNode = &node;
            ImGui::SetDragDropPayload("Drag Node", &pDragNode, sizeof(Transform*));
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload =
                    ImGui::AcceptDragDropPayload("Drag Node"))
            {
                pDragNode->SetParent(node);
            }
            ImGui::EndDragDropTarget();
        }
    };

    auto TreeRightClickEvent = [&node]() 
    {
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
                GameObject::Destroy(node.gameObject);
            }
            ImGui::EndPopup();
        }
    };

    auto PushFocusStyle = [&node](GameObject* pFocusObject) 
    {
        if (&pFocusObject->transform == &node)
        {           
            ImGui::PushStyleColor(ImGuiCol_Text,
                                  ImVec4(0.4f, 0.75f, 1.0f, 1.0f)); // 글자색
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered,
                                  ImVec4(0.2f, 0.45f, 0.8f, 1.0f)); // 포커스시
            ImGui::PushStyleColor(ImGuiCol_HeaderActive,
                                  ImVec4(0.25f, 0.55f, 0.9f, 1.0f)); // 클릭시
            return true;
        }
        return false;
    };
    auto PopFocusStyle = [&node](GameObject* pFocusObject) 
    {
        if (&pFocusObject->transform == &node)
        {
            ImGui::PopStyleColor(3);

            return true;
        }
        return false;
    };


    ImGui::PushID(&node);
    PushFocusStyle(focusObject.get());
    if (ImGui::TreeNodeEx(node.gameObject.ToString().data(),
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

ObjectTestEditor::ObjectTestEditor()
{
    using namespace u8_literals;
    SetLabel(u8"하이러키 테스트용"_c_str);
}

ObjectTestEditor::~ObjectTestEditor()
{
}

void ObjectTestEditor::OnStartGui()
{

}

void ObjectTestEditor::OnPreFrame()
{

}

static std::string MakeCountName(std::string_view baseName)
{
    size_t                    count   = 0;
    std::string               name    = baseName.data();
    std::weak_ptr<GameObject> pObject = GameObject::Find(name);
    while (pObject.expired() == false)
    {
        name    = std::format("{} ({})", baseName.data(), count++);
        pObject = GameObject::Find(name);
    }
    return name;
}

void ObjectTestEditor::OnFrame()
{
    if (ImGui::Button(u8"Empty Scene Load"_c_str))
    {
        engineCore->SceneManager.LoadScene("EmptyScene");
    }

    if (ImGui::Button(u8"오브젝트 만들기"_c_str))
    {
        int count = 0;
        std::string name  = MakeCountName("Game Object");
        NewGameObject<GameObject>(name);
    }

    const auto& scenes = engineCore->SceneManager.GetBuildScenes();
    for (auto& [sceneName, scenes] : scenes)
    {
        if (scenes.isLoaded == false)
            continue;

        std::string sName = sceneName;
        if (ImGui::CollapsingHeader(sName.c_str()))
        {
            auto rootObjects = scenes.GetRootGameObjects();
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
}

void ObjectTestEditor::OnPostFrame()
{

}
