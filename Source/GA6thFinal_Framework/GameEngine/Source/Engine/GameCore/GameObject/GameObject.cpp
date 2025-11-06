#include "pch.h"
#include "Editor/Tool/Scene/Command/EditorSceneCommands.h"
using namespace Global;

REFLECT_FUNCTION(GameObject)

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

std::vector<std::weak_ptr<GameObject>> GameObject::FindGameObjectsWithTag(const std::string& tag)
{
    return ESceneManager::Engine::FindGameObjectsWithTag(tag);
}

std::weak_ptr<GameObject> GameObject::FindWithTag(const std::string& tag)
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
    _instanceID(-1),
    _activeInHierarchy(true), 
    _creationFrame(0)
{

}

GameObject::~GameObject()
{
    _components.clear();
    if (0 <= _instanceID)
    {
        UmGameObjectFactory.InstanceID.ReturnInstanceID(_instanceID);
        _instanceID = -1;
        _ownerScene = STR_NULL;
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

void GameObject::OnInspectorBegin() 
{
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("Paste Component", "V") || ImGui::IsKeyReleased(ImGuiKey_V))
        {
            std::wstring clipboardText = File::GetClipboardText();
            if (false == clipboardText.empty())
            {
                std::string yamlData = WStringToU8(clipboardText);
                try
                {
                    auto [node, result] = YAMLHelper::SafeLoad(yamlData);
                    if (result)
                    {
                        UmComponentFactory.AddComponentToYamlLifeCycle(this, &node);
                    }
                }
                catch (const YAML::Exception)
                {

                }
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
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
            if (false == UmCore->IsPlay())
            {
                Scene* ownerScene = UmSceneManager.GetSceneByName(_ownerScene);
                if (ownerScene)
                {
                    ownerScene->IsDirty = true;
                }
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
            ImGui::Text("ActiveInHierarchy : %s", _activeInHierarchy ? "true" : "false");
            ImGui::Text("Scene : %s", GetOwnerSceneName().data());
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
            const std::vector<std::shared_ptr<GameObject>>* originPrefab = nullptr;
            if (pPrefabObject != nullptr)
            {
                originPrefab = UmGameObjectFactory.GetOriginPrefab(pPrefabObject->_prefabGuid);
            }

            //안전한 순회 및 동적 할당 반복 방지를 위한 thread_local 순회용 컨테이너
            static thread_local std::vector<Component*> components;
            components.clear();
            for (auto& component : _components)
            {
                components.push_back(component.get());
            }

            size_t componentsCount = components.size();
            for (int i = 0; i < componentsCount; i++)
            {
                Component* component = components[i];
                ImGui::PushID(component);
                {
                    const char* className = component->ClassName();
                    auto componentContextPopup = [&]() 
                    {
                        if (ImGui::BeginPopupContextItem("Component Context Popup"))
                        {
                            if (ImGui::MenuItem("Copy Component"))
                            {
                                YAML::Emitter componentYamlEmitter;
                                componentYamlEmitter << UmComponentFactory.SerializeToYaml(component);
                                std::wstring componentData = U8ToWString(componentYamlEmitter.c_str());
                                File::SetClipboardText(componentData);
                            }
                            if (ImGui::MenuItem("Destroy Component"))
                            {
                                if (false == editorModule->PlayMode.IsPlay())
                                {
                                    UmCommandManager.Do<Command::EditorScene::DestroyComponentCommand>(component);
                                }
                                else
                                {
                                    GameObject::Destroy(component);
                                }
                            }
                            ImGui::EndPopup();
                        }
                    };

                    if (ImGui::CollapsingHeader(className, ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        componentContextPopup();
                        if (originPrefab != nullptr)
                        {
                            // 오버라이딩 맴버 여부 확인
                            static std::unordered_map<std::string, std::pair<std::string, void*>> overrideMap;
                            int reflectFieldsCount = 0;
                            overrideMap.clear();
                            component->applyReflectFields([&](std::string_view rflName, void* pData) {
                                std::string_view propertyName;
                                if (true == UmGameObjectFactory.IsOverrideField(pData, &propertyName))
                                {
                                    overrideMap[propertyName.data()] = std::make_pair(rflName.data(), pData);
                                }
                                reflectFieldsCount++;
                            });

                            // Override 검증용
                            UmCore->ImGuiDrawPropertysSetting.InputEndEvent = [&](bool result, std::string_view name) {
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
                                                auto& originPrefabs = (*originPrefab);
                                                if (myNumber < originPrefabs.size())
                                                {
                                                    GameObject* prefab = originPrefabs[myNumber].get();
                                                    Component*  prefabComponent = prefab->GetComponentAtIndex<Component>(i);
                                                    if (prefabComponent != nullptr)
                                                    {
                                                        std::string prefabData =
                                                            prefabComponent->SerializedReflectFields();
                                                        yyjson_doc* prefabDoc =
                                                            yyjson_read(prefabData.c_str(), prefabData.size(), 0);
                                                        yyjson_val* prefabRoot = yyjson_doc_get_root(prefabDoc);

                                                        std::string myData = component->SerializedReflectFields();
                                                        yyjson_doc* myDoc =
                                                            yyjson_read(myData.c_str(), myData.size(), 0);
                                                        yyjson_val* myRoot = yyjson_doc_get_root(myDoc);

                                                        component->applyReflectFields([&](std::string_view rflName,
                                                                                          void*            pData) {
                                                            yyjson_val* prefabVal =
                                                                yyjson_obj_get(prefabRoot, rflName.data());
                                                            char* prefabCStr = yyjsonValToCStr(prefabVal);

                                                            yyjson_val* myVal  = yyjson_obj_get(myRoot, rflName.data());
                                                            char*       myCStr = yyjsonValToCStr(myVal);

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
                                }

                                auto overrideIter = overrideMap.find(name.data());
                                if (overrideIter != overrideMap.end())
                                {
                                    auto& [name, pair]     = *overrideIter;
                                    auto& [rflName, pData] = pair;
                                    ImGui::PushID(pData);
                                    {
                                        ImGui::SameLine();
                                        if (ImGui::Button("Revert"))
                                        {
                                            UmGameObjectFactory.UnsetOverrideFlag(pData);
                                            UmComponentFactory.RevertOverrideField(component, rflName.data());
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
                        static EditorSceneTool* editorSceneTool = EditorSceneTool::GetTool();
                        if (nullptr != editorSceneTool)
                        {
                            if (editorSceneTool->DrawGizmo)
                            {
                                component->OnDrawDebugSelected();        
                            }                                            
                        }         
                        else
                        {
                            assert(!"에디터 없어요");
                        }
                    }
                    else
                    {
                        componentContextPopup();
                    }

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
    if (ImGui::TreeNodeEx("Tags", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
    {
        static std::vector<std::string> eraseList;
        for (auto iter = ReflectFields->_tags.begin(); iter != ReflectFields->_tags.end(); ++iter)
        {
            ImGui::BulletText("%s", iter->c_str());
            ImGui::SameLine();
            std::string buttonLabel = "Remove##" + *iter;
            if (ImGui::SmallButton(buttonLabel.c_str()))
            {
                eraseList.push_back(*iter);
            }                
        }

        if (false == eraseList.empty())
        {
            for (auto& key : eraseList)
            {
                RemoveTag(key);
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
                AddTag(tagInputBuffer);
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
        ImGui::TreePop();
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

std::weak_ptr<ITimeInvoker> GameObject::GetWeakInvoker()
{
    auto ptr = GetWeakPtr().lock();
    return std::weak_ptr<ITimeInvoker>(ptr);
}

bool GameObject::AddTag(const std::string& tag)
{
    auto [iter, result] = ReflectFields->_tags.insert(tag);
    if (true == result)
    {
        ESceneManager::Engine::InsertGameObjectTag(this, tag);
    }
    return result;
}

bool GameObject::RemoveTag(const std::string& tag)
{
    ESceneManager::Engine::EraseGameObjectTag(this, tag);
    auto result = ReflectFields->_tags.erase(tag);
    return 0 < result;
}

bool GameObject::CompareTag(const std::string& tag)
{
    bool result = ReflectFields->_tags.find(tag) != ReflectFields->_tags.end();
    return result;
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

void GameObject::Engine::ResetActiveInHierarchy(GameObject* obj) 
{
    Transform* curr = &obj->_transform;
    curr->gameObject->_activeInHierarchy = obj->IsValid();
    if (true == curr->gameObject->_activeInHierarchy)
    {
        while (curr != nullptr)
        {
            if (false == curr->gameObject->ReflectFields->_activeSelf)
            {
                curr->gameObject->_activeInHierarchy = false;
                break;
            }
            curr = curr->Parent;
        }
    }

    for (auto& component : obj->_components)
    {
        component->UpdateEnableInHierarchy();
        component->_prevFrameEnableInHierarchy = component->_enableInHierarchy;
    }
}

void GameObject::Engine::UpdateActiveInHierarchy(GameObject* obj)
{
    Transform::ForeachBFS(obj->_transform, [](Transform* currTr) 
    {
        GameObject* curr = &currTr->gameObject;
        Transform* parent = currTr->Parent;
        bool parentActiveInHierarchy = parent ? parent->gameObject->_activeInHierarchy : true;
        bool prevActive = curr->_activeInHierarchy;
        bool currActive = parentActiveInHierarchy && curr->ReflectFields->_activeSelf && curr->IsValid();
        if (prevActive != currActive)
        {
            if (curr->IsValid())
            {
                curr->_activeInHierarchy = currActive;
            }
            else
            {
                curr->_activeInHierarchy = false;
            }

            for (auto& component : curr->_components)
            {
                component->UpdateEnableInHierarchy();
            }
        }     
    });
}
