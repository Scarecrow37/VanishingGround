#include "pch.h"
using namespace Global;

#define SAFE_FREE(ptr) if(ptr != nullptr) free(ptr)

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
    _ownerScene(STR_NULL),
    _prefabGuid(STR_NULL),
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
    if (_ownerScene != ESceneManager::DONT_DESTROY_ON_LOAD_SCENE_NAME)
    {
        return *engineCore->SceneManager.GetSceneByName(_ownerScene);       
    }
    else
    {
        return *engineCore->SceneManager.GetDontDestroyOnLoadScene();
    }
}

void GameObject::OnInspectorViewEnter() 
{

}

void GameObject::OnInspectorStay() 
{
    using namespace u8_literals;
    constexpr ImVec4 DEBUG_TEXT_COLOR = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
    constexpr ImVec4 DEBUG_FRAMEBG_COLOR = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    static GameObject* selectObject = nullptr;
    static bool isDebug  = false;
    auto SetSceneDirtyFlag = [this](bool result, std::string_view name)
    {
        if (result)
        {
            Scene* ownerScene = UmSceneManager.GetSceneByName(_ownerScene);
            if (ownerScene)
            {
                ownerScene->IsDirty = true;
            }
        }
    };

    ImGui::PushID(this);
    {
        ImGui::Checkbox("Debug", &isDebug);
        ImGui::Separator();
        bool isPrefab = IsPrefabInstance();
        GameObject* pPrefabObject = PrefabInstance; 
        if (isPrefab)
        {
            ImGui::Text("Prefab");
            ImGui::PushStyleColor(ImGuiCol_Text, DEBUG_TEXT_COLOR);   
            ImGui::PushStyleColor(ImGuiCol_FrameBg, DEBUG_FRAMEBG_COLOR);
            static std::string guidPath;
            guidPath = _prefabGuid.ToPath().string();
            if (guidPath.empty() == false)
            {
                ImGui::InputText("Prefab file path", &guidPath, ImGuiInputTextFlags_ReadOnly);
            }
            else
            {
                static std::string emptyPath = STR_NULL;
                ImGui::InputText("Prefab file path", &emptyPath, ImGuiInputTextFlags_ReadOnly);
            }     

            if (isDebug)
            {
                static std::string tempPath;
                if (_prefabGuid != tempPath)
                {
                    tempPath = _prefabGuid.string();
                }          
                ImGui::InputText("Prefab GUID", &tempPath, ImGuiInputTextFlags_ReadOnly);
            }
            ImGui::PopStyleColor(2);
            ImGui::Separator();
        }

        if (isDebug)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, DEBUG_TEXT_COLOR);   
            ImGui::Text("Instance ID : %d", _instanceID);
            ImGui::PopStyleColor();
        }

        UmCore->ImGuiDrawPropertysSetting.InputEndEvent = SetSceneDirtyFlag;
        ImGuiDrawPropertys();

        UmCore->ImGuiDrawPropertysSetting.InputEndEvent = SetSceneDirtyFlag;
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

        for (int i = 0; i < _components.size(); i++)
        {
            std::shared_ptr<Component>& component = _components[i];
            ImGui::PushID(component.get());
            {
                ImGui::Separator();
                const char* className = component->ClassName();
                ImGui::Text(className);
                ImGui::SameLine();
                ImGui::Text(" Component");
                if (pPrefabObject != nullptr)
                {
                    UmCore->ImGuiDrawPropertysSetting.InputEndEvent = [&](bool result, std::string_view name) 
                    {
                        if (result == true)
                        {                 
                            SetSceneDirtyFlag(result, name);
                            const auto* originPrefab = UmGameObjectFactory.GetOriginPrefab(pPrefabObject->_prefabGuid);
                            if (originPrefab != nullptr)
                            {
                                int myNumber = -1;
                                int level    = 0;
                                Transform::ForeachBFS(pPrefabObject->_transform, [&](Transform* curr) {
                                    if (curr == &_transform)
                                    {
                                        myNumber = level;
                                    }
                                    level++;
                                });                            
                                if (myNumber > -1)
                                {
                                    using namespace ReflectHelper::json;
                                    GameObject* prefab = (*originPrefab)[myNumber].get();
                                    Component* prefabComponent = prefab->GetComponentAtIndex<Component>(i);
                                    if (prefabComponent != nullptr)
                                    {
                                        std::string prefabData = prefabComponent->SerializedReflectFields();
                                        yyjson_doc* prefabDoc  = yyjson_read(prefabData.c_str(), prefabData.size(), 0);
                                        yyjson_val* prefabRoot = yyjson_doc_get_root(prefabDoc);

                                        std::string myData = component->SerializedReflectFields();
                                        yyjson_doc* myDoc  = yyjson_read(myData.c_str(), myData.size(), 0);
                                        yyjson_val* myRoot = yyjson_doc_get_root(myDoc);

                                        component->applyReflectFields([&](std::string_view rflName, void* pData) 
                                        {
                                            yyjson_val* prefabVal = yyjson_obj_get(prefabRoot, name.data());
                                            char* prefabCStr = yyjsonValToCStr(prefabVal);

                                            yyjson_val* myVal  = yyjson_obj_get(myRoot, name.data());
                                            char* myCStr = yyjsonValToCStr(myVal);

                                            if (prefabCStr != nullptr && myCStr && nullptr)
                                            {
                                                if (0 != std ::strcmp(prefabCStr, myCStr))
                                                {
                                                    // std::string message = std::format("edit {}", name);
                                                    // UmLogger.Log(LogLevel::LEVEL_TRACE, message);
                                                }

                                                SAFE_FREE(prefabCStr);
                                                SAFE_FREE(myCStr);
                                            }
                                        });
                                        yyjson_doc_free(prefabDoc);
                                    }
                                }
                            }
                        }
                    };
                }
                else
                {
                    UmCore->ImGuiDrawPropertysSetting.InputEndEvent = SetSceneDirtyFlag;
                }
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

void GameObject::SerializedReflectEvent() 
{
   
}

void GameObject::DeserializedReflectEvent() 
{
   
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