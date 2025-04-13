#include "pch.h"
using namespace Global;

void GameObject::DontDestroyOnLoad(GameObject& gameObject)
{
    ESceneManager::Engine::DontDestroyOnLoadObject(gameObject);
}

void GameObject::Instantiate(GameObject& gameObject)
{

}

void GameObject::Destroy(Component& component, float t)
{
    ESceneManager::Engine::DestroyObject(component);
}

void GameObject::Destroy(GameObject& gameObject, float t)
{
    ESceneManager::Engine::DestroyObject(gameObject);
}

GameObject::GameObject()
    : 
    transform(*this),
    _ownerScene("null"),
    _components(),
    _instanceID(-1)
{

}

GameObject::~GameObject()
{
    _components.clear();
}

Scene& GameObject::GetScene()
{
    return *engineCore->SceneManager.GetSceneByName(_ownerScene);
}

std::weak_ptr<GameObject> GameObject::GetWeakPtr() const
{
    return ESceneManager::Engine::GetRuntimeObjectWeakPtr(_instanceID);
}

void GameObject::OnFocusInspectorView() 
{

}

void GameObject::OnDrawInspectorView() 
{
    using namespace u8_literals;

    static GameObject* selectObject = nullptr;
    ImGui::PushID(this);
    {
        ImGuiDrawPropertys();
        transform.ImGuiDrawPropertys();
        if (ImGui::Button("AddComponent"))
        {
            selectObject = this;
            ImGui::OpenPopup("Select Component");
        }
        ImGui::SameLine();
        if (ImGui::Button("Destroy GameObject"))
        {
            if (selectObject == this)
                selectObject = nullptr;

            GameObject::Destroy(this);
        }

        for (auto& component : _components)
        {
            ImGui::PushID(component.get());
            {
                ImGui::Separator();
                ImGui::Text(component->ClassName());
                component->ImGuiDrawPropertys();
                ImGui::SameLine();
                if (ImGui::Button("Destroy Component"))
                {
                    GameObject::Destroy(component.get());
                }
                ImGui::Separator();
            }
            ImGui::PopID();
        }

        if (selectObject)
        {
            if (ImGui::BeginPopupModal("Select Component", 
                                       nullptr,
                                       ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::BeginChild("ScrollRegion", 
                                  ImVec2(300, 200), 
                                  0,
                                  ImGuiWindowFlags_HorizontalScrollbar);
                for (auto& key :
                     engineCore->ComponentFactory.GetNewComponentKeyList())
                {
                    if (ImGui::Button(key.c_str()))
                    {
                        engineCore->ComponentFactory.AddComponentToObject(
                            selectObject, 
                            key);
                        ImGui::CloseCurrentPopup();
                    }
                }
                ImGui::EndChild();

                if (ImGui::Button("Cancel"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                ImGui::EndPopup();
            }
        }
    }
    ImGui::PopID();
}


