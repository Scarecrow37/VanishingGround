#include "ObjectTestEditor.h"

using namespace u8_literals;
using namespace Global;

static void TestTreeNode(const Transform& node)
{
    ImGui::PushID(node.gameObject.GetInstanceID());
    if (ImGui::TreeNode(node.gameObject.ToString().data()))
    {
        for (int i = 0; i < node.ChildCount; i++)
        {
            Transform& child = *node.GetChild(i);
            TestTreeNode(child); // 재귀 호출
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

ObjectTestEditor::ObjectTestEditor()
{
    using namespace u8_literals;
    SetLabel(u8"오브젝트 테스트용"_c_str);
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

    static GameObject* selectObj = nullptr;
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
                    ImGui::Text("%s", obj->ToString().data());
                    if (ImGui::Button("DontDestroy"))
                    {
                        GameObject::DontDestroyOnLoad(obj.get());
                    }

                    obj->transform.ImGuiDrawPropertys();                    
                    static std::string transformData;
                    if (ImGui::Button("save"))
                    {
                        transformData = obj->transform.SerializedReflectFields();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("load"))
                    {
                        obj->transform.DeserializedReflectFields(transformData);
                    }
                    ImGui::SameLine();
                    ImGui::Text(transformData.c_str());

                    if (ImGui::Button("AddComponent"))
                    {
                        ImGui::OpenPopup("Select Component");
                        selectObj = obj.get();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Destroy"))
                    {
                        GameObject::Destroy(obj.get());
                    }
                    obj->ImGuiDrawPropertys();

                    for (size_t i = 0; i < obj->GetComponentCount(); i++)
                    {
                        if (auto component = obj->GetComponentAtIndex<Component>(i).lock())
                        {
                            static std::string componentData;

                            ImGui::PushID(component.get());
                            ImGui::Separator();
                            {     
                                ImGui::Text("%s", component->ClassName());
                                component->ImGuiDrawPropertys();
                                if (ImGui::Button("Destroy"))
                                {
                                    GameObject::Destroy(component.get());
                                }
                                if (ImGui::Button("save"))
                                {
                                    componentData = component->SerializedReflectFields();
                                }
                                ImGui::SameLine();
                                if (ImGui::Button("load"))
                                {
                                    component->DeserializedReflectFields(componentData);
                                }
                                ImGui::SameLine();
                                ImGui::Text(componentData.c_str());
                            }
                            ImGui::Separator();
                            ImGui::PopID();
                        }
                    }
                }
                ImGui::PopID();
            }
        }
    }

    if (selectObj)
    {
        ImGui::PushID(selectObj);
        if (ImGui::BeginPopupModal("Select Component", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::BeginChild("ScrollRegion", ImVec2(300, 200), 0, ImGuiWindowFlags_HorizontalScrollbar);
            for (auto& key :
                 engineCore->ComponentFactory.GetNewComponentKeyList())
            {
                if (ImGui::Button(key.c_str()))
                {
                    engineCore->ComponentFactory.AddComponentToObject(selectObj, key);
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndChild();
            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            ImGui::EndPopup();
        }
        ImGui::PopID();
    }
}

void ObjectTestEditor::OnPostFrame()
{

}
