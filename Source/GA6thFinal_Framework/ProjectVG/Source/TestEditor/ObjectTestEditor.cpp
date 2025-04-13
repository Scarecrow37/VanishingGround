#include "ObjectTestEditor.h"
#include "Source/EditorTools/EditorInspectorView.h"

using namespace u8_literals;
using namespace Global;

static void TransformTreeNode(Transform& node)
{
    auto TreeDoubleClickEvent = [&node]() 
    {
        bool result = ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered();
        if (result)
        {
            EditorInspectorView::SetFocusObject(node.gameObject.GetWeakPtr());
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

    ImGui::PushID(&node);
    if (ImGui::TreeNodeEx(node.gameObject.ToString().data(),
                          ImGuiTreeNodeFlags_OpenOnArrow))
    {
        TreeDoubleClickEvent();
        TreeRightClickEvent();
        TreeDragDropEvent();

        for (int i = 0; i < node.ChildCount; i++)
        {
            Transform* child = node.GetChild(i);
            if (child)
            {
                TransformTreeNode(*child); // 재귀 호출
            }      
        }  
        ImGui::TreePop();
    }
    else
    {
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

void ObjectTestEditor::OnFrame()
{
    if (ImGui::Button(u8"Empty Scene Load"_c_str))
    {
        engineCore->SceneManager.LoadScene("EmptyScene");
    }

    if (ImGui::Button(u8"오브젝트 만들기"_c_str))
    {
        NewGameObject<GameObject>("Game Object");
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
            for (auto& obj : rootObjects)
            {
                ImGui::PushID(obj.get());
                {      
                    TransformTreeNode(obj->transform);
                }
                ImGui::PopID();
            }
        }
    }
}

void ObjectTestEditor::OnPostFrame()
{

}
