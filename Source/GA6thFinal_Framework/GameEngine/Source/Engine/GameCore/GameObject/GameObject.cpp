#include "pch.h"
#include "Editor/Tool/Scene/Command/EditorSceneCommands.h"
using namespace Global;

#define SAFE_FREE(ptr) if(ptr != nullptr) free(ptr)

void GameObject::DontDestroyOnLoad(GameObject& gameObject)
{
    ESceneManager::Engine::DontDestroyOnLoadObject(gameObject);
}

GameObject* GameObject::Instantiate(GameObject& gameObject)
{
    YAML::Node node = UmGameObjectFactory.SerializeToYaml(&gameObject);
    auto pObject = UmGameObjectFactory.DeserializeToYaml(&node);
    return pObject.get();
}

std::vector<std::weak_ptr<GameObject>> GameObject::FindGameObjectsWithTag(std::string_view tag)
{
    return ESceneManager::Engine::FindGameObjectsWithTag(tag);
}

std::weak_ptr<GameObject> GameObject::FindWithTag(std::string_view tag)
{
    return ESceneManager::Engine::FindGameObjectWithTag(tag);
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
    if (0 <= _instanceID)
    {
        UmGameObjectFactory.InstanceID.ReturnInstanceID(_instanceID);
    }

    if (0 < ReflectFields->_tags.size())
    {
        for (auto& tag : ReflectFields->_tags)
        {
            ESceneManager::Engine::EraseGameObjectTag(this, tag);
        }
    }
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

void GameObject::OnInspectorEnter() 
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
        if (ImGui::Button("Destroy GameObject"))
        {
            if (selectObject == this)
                selectObject = nullptr;

            if (false == editorModule->PlayMode.IsPlay())
            {
                UmCommandManager.Do<Command::EditorScene::DestroyGameObjectCommand>(this);
            }
            else
            {
                GameObject::Destroy(this);
            }
        }
        ImGui::SameLine();
        ImGui::Checkbox("Debug", &isDebug);

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
        }

        if (isDebug)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, DEBUG_TEXT_COLOR);   
            ImGui::Text("Instance ID : %d", _instanceID);
            ImGui::PopStyleColor();
        }

        ImguiEditTags();

        UmCore->ImGuiDrawPropertysSetting.InputEndEvent = SetSceneDirtyFlag;
        ImGuiDrawPropertys();

        UmCore->ImGuiDrawPropertysSetting.InputEndEvent = SetSceneDirtyFlag;
        _transform.ImGuiDrawPropertys();

        if (ImGui::Button("AddComponent"))
        {
            selectObject = this;
            ImGui::OpenPopup("Add Component");
        }
        ImGui::Separator();

        if (false == _components.empty())
        {
            size_t componentsCount = _components.size();
            const std::vector<std::shared_ptr<GameObject>>* originPrefab = nullptr;
            if (pPrefabObject != nullptr)
            {
                originPrefab = UmGameObjectFactory.GetOriginPrefab(pPrefabObject->_prefabGuid);
            }

            for (int i = 0; i < componentsCount; i++)
            {
                std::shared_ptr<Component>& component = _components[i];
                ImGui::PushID(component.get());
                {
                    const char* className = component->ClassName();
                    ImGui::Text(className);
                    ImGui::SameLine();
                    if (ImGui::Button("Destroy Component"))
                    {
                        if (false == editorModule->PlayMode.IsPlay())
                        {
                            UmCommandManager.Do<Command::EditorScene::DestroyComponentCommand>(component.get());
                        }
                        else
                        {
                            GameObject::Destroy(component.get());
                        }
                    }

                    if (originPrefab != nullptr)
                    {
                        // 오버라이딩 맴버 여부 확인
                        static std::unordered_map<std::string, std::pair<std::string, void*>> overrideMap;
                        int reflectFieldsCount = 0;
                        overrideMap.clear();
                        component->applyReflectFields([&](std::string_view rflName, void* pData) 
                        {
                            std::string_view propertyName;
                            if (true == UmGameObjectFactory.IsOverrideField(pData, &propertyName))
                            {
                                overrideMap[propertyName.data()] = std::make_pair(rflName.data(), pData);
                            }
                            reflectFieldsCount++;
                        });

                        //Override 검증용
                        UmCore->ImGuiDrawPropertysSetting.InputEndEvent = [&](bool result, std::string_view name) 
                        {
                            if (result == true)
                            {
                                SetSceneDirtyFlag(result, name);
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
                                        if (overrideMap.size() < reflectFieldsCount)
                                        {
                                            using namespace ReflectHelper::json;
                                            GameObject* prefab = (*originPrefab)[myNumber].get();
                                            Component*  prefabComponent = prefab->GetComponentAtIndex<Component>(i);
                                            if (prefabComponent != nullptr)
                                            {
                                                std::string prefabData = prefabComponent->SerializedReflectFields();
                                                yyjson_doc* prefabDoc =
                                                yyjson_read(prefabData.c_str(), prefabData.size(), 0);
                                                yyjson_val* prefabRoot = yyjson_doc_get_root(prefabDoc);

                                                std::string myData = component->SerializedReflectFields();
                                                yyjson_doc* myDoc  = yyjson_read(myData.c_str(), myData.size(), 0);
                                                yyjson_val* myRoot = yyjson_doc_get_root(myDoc);

                                                component->applyReflectFields([&](std::string_view rflName, void* pData) 
                                                {
                                                    yyjson_val* prefabVal = yyjson_obj_get(prefabRoot, rflName.data());
                                                    char* prefabCStr = yyjsonValToCStr(prefabVal);

                                                    yyjson_val* myVal  = yyjson_obj_get(myRoot, rflName.data());
                                                    char* myCStr = yyjsonValToCStr(myVal);

                                                    if (prefabCStr != nullptr && myCStr != nullptr)
                                                    {
                                                        if (0 != std ::strcmp(prefabCStr, myCStr))
                                                        {
                                                            UmGameObjectFactory.SetOverrideFlag(pData, name);
                                                        }

                                                        SAFE_FREE(prefabCStr);
                                                        SAFE_FREE(myCStr);
                                                    }
                                                });

                                                yyjson_doc_free(prefabDoc);
                                                yyjson_doc_free(myDoc);
                                            }
                                        }
                                    }
                                }
                            }

                            auto overrideIter = overrideMap.find(name.data());
                            if (overrideIter != overrideMap.end())
                            {
                                auto& [name, pair] = *overrideIter;
                                auto& [rflName, pData] = pair;
                                ImGui::PushID(pData);
                                {
                                    ImGui::SameLine();
                                    if (ImGui::Button("Revert"))
                                    {
                                        UmGameObjectFactory.UnsetOverrideFlag(pData);
                                        UmComponentFactory.RevertOverrideField(component.get(), rflName.data());
                                        GetScene().IsDirty = true;
                                    }
                                }
                                ImGui::PopID();
                            }
                        };
                    }
                    else
                    {
                        UmCore->ImGuiDrawPropertysSetting.InputEndEvent = SetSceneDirtyFlag;
                    }
                    component->ImGuiDrawPropertys();

                    ImGui::Separator();
                }
                ImGui::PopID();
            }
        }

        if (selectObject)
        {
            constexpr ImVec2 popupSize(400, 300);
            ImGui::SetNextWindowSize(popupSize, ImGuiCond_FirstUseEver); 
            if (ImGui::BeginPopupModal("Add Component", nullptr))
            {
                ImGui::BeginChild("Component Child", 
                                  ImVec2{0, ImGui::GetContentRegionAvail().y - 40.0f}, 
                                  ImGuiChildFlags_AutoResizeX,
                                  ImGuiWindowFlags_HorizontalScrollbar);
                if (UmComponentFactory.HasScript() == true)
                {
                    static ImGuiTextFilter filter;
                    filter.Draw("Search");

                    static ImVec2 popupPos{};
                    static std::string inputBuffer{};
                    if (ImGui::Selectable(u8"스크립트 파일 만들기"_c_str))
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
                        if (filter.PassFilter(key.c_str() + 6))
                        {
                            if (ImGui::Selectable(key.c_str() + 6))
                            {
                                UmCommandManager.Do<Command::EditorScene::AddComponentCommand>(selectObject, key);
                                ImGui::CloseCurrentPopup();
                            }
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

void GameObject::ImguiEditTags() 
{
    if (ImGui::CollapsingHeader("Tags", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
    {
        static std::vector<std::set<std::string>::iterator> eraseList;
        for (auto iter = ReflectFields->_tags.begin(); iter != ReflectFields->_tags.end(); ++iter)
        {
            ImGui::BulletText("%s", iter->c_str());
            ImGui::SameLine();
            std::string buttonLabel = "Remove##" + *iter;
            if (ImGui::SmallButton(buttonLabel.c_str()))
            {
                eraseList.push_back(iter);
            }                
        }

        if (false == eraseList.empty())
        {
            for (auto& iter : eraseList)
            {
                ESceneManager::Engine::EraseGameObjectTag(this, *iter);
                ReflectFields->_tags.erase(iter);
            }
            eraseList.clear();

            GetScene().IsDirty = true;
        }

        static std::string tagInputBuffer;
        if (ImGui::Button("Add"))
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            ImGui::SetNextWindowPos(mousePos, ImGuiCond_Always);         
            ImGui::OpenPopup("AddTagsPopup");
        }

        if (ImGui::BeginPopup("AddTagsPopup"))
        {
            ImGui::InputText("##Tags", &tagInputBuffer);

            if (ImGui::IsKeyReleased(ImGuiKey_Enter) || ImGui::Button("Add"))
            {                
                auto [iter, result] = ReflectFields->_tags.insert(tagInputBuffer);
                if (result)
                {
                    ESceneManager::Engine::InsertGameObjectTag(this, tagInputBuffer);
                }
                ImGui::CloseCurrentPopup();

                GetScene().IsDirty = true;
            }
            ImGui::SameLine();
            if (ImGui::IsKeyReleased(ImGuiKey_Escape) || ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    ImGui::Separator();
}

void GameObject::SerializedReflectEvent() 
{
   
}

void GameObject::DeserializedReflectEvent() 
{
    //태그들을 SceneManager에 등록
    if (true == IsValid())
    {
        for (auto& tag : ReflectFields->_tags)
        {
            ESceneManager::Engine::InsertGameObjectTag(this, tag);
        }
    }
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