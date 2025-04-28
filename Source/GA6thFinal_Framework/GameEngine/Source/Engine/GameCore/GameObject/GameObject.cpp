#include "pch.h"
using namespace Global;

void GameObject::DontDestroyOnLoad(GameObject& gameObject)
{
    ESceneManager::Engine::DontDestroyOnLoadObject(gameObject);
}

void GameObject::Instantiate(GameObject& gameObject)
{
    YAML::Node node = UmGameObjectFactory.SerializeToYaml(&gameObject);
    UmGameObjectFactory.DeserializeToYaml(&node);
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
    _transform(*this),
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
    return _weakPtr;
}

void GameObject::OnInspectorViewEnter() 
{

}

void GameObject::OnInspectorStay() 
{
    using namespace u8_literals;

    static GameObject* selectObject = nullptr;
    ImGui::PushID(this);
    {
        ImGuiDrawPropertys();
        _transform.ImGuiDrawPropertys();
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
            constexpr ImVec2 popupSize(400, 300);
            ImGui::SetNextWindowSize(popupSize, ImGuiCond_FirstUseEver); 
            if (ImGui::BeginPopupModal("Select Component", nullptr))
            {
                ImGui::BeginChild("Component Child", 
                                  ImVec2{0, ImGui::GetContentRegionAvail().y - 40.0f}, 
                                  0,
                                  ImGuiWindowFlags_HorizontalScrollbar);
                if (UmComponentFactory.HasScript() == true)
                {
                    static ImVec2      popupPos{};
                    static std::string inputBuffer{};
                    if (ImGui::Button(u8"스크립트 파일 만들기"_c_str))
                    {
                        popupPos = ImGui::GetMousePos();
                        inputBuffer.clear();
                        ImGui::OpenPopup(u8"스크립트 생성 팝업"_c_str);
                    }

                    if (ImGui::BeginPopup(u8"스크립트 생성 팝업"_c_str))
                    {
                        ImGui::SetNextWindowPos(popupPos, ImGuiCond_Appearing);
                        ImGui::Text(u8"컴포넌트 이름을 입력하세요."_c_str);
                        ImGui::Text(u8"예) MyTest/MyFirstComponent"_c_str);
                        ImGui::InputText("##new_script_file_name", &inputBuffer);
                        if (ImGui::Button("OK"))
                        {
                            UmComponentFactory.MakeScriptFile(inputBuffer.c_str());
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Cancel"))
                        {
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }

                    for (auto& key : engineCore->ComponentFactory.GetNewComponentKeyList())
                    {
                        if (ImGui::Button(key.c_str() + 6))
                        {
                            engineCore->ComponentFactory.AddComponentToObject(selectObject, key);
                            ImGui::CloseCurrentPopup();
                        }
                    }
                }
                else
                {
                    ImGui::Text(u8"스크립트를 Build 해주세요. :("_c_str);
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

std::string GameObject::Helper::GenerateUniqueName(std::string_view baseName)
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