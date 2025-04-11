#include "ObjectTestEditor.h"
#include "Source/EditorTools/EditorInspectorView.h"

using namespace u8_literals;
using namespace Global;

static void TransformTreeNode(const Transform& node)
{
    ImGui::PushID(&node);
    if (ImGui::TreeNodeEx(node.gameObject.ToString().data(),
                          ImGuiTreeNodeFlags_OpenOnArrow))
    {
        for (int i = 0; i < node.ChildCount; i++)
        {
            Transform& child = *node.GetChild(i);
            TransformTreeNode(child); // 재귀 호출
        }
        ImGui::TreePop();
    }
    ImGui::SameLine();
    if (ImGui::Button("Select Inspector"))
    {
        EditorInspectorView::SetFocusObject(node.gameObject.GetWeakPtr());
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
