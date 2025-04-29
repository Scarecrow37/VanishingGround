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
        bool isPrefab = IsPrefabInstacne;
        const YAML::Node* pPrefabData = nullptr;
        std::string prefabReflectJson;
        yyjson_doc* prefabDoc = nullptr;
        yyjson_val* prefabRoot = nullptr;

        std::string thisReflectJson;
        yyjson_doc* thisDoc = nullptr;
        yyjson_val* thisRoot = nullptr;
        if (isPrefab)
        {
            pPrefabData = UmGameObjectFactory.GetPrefabData(this);
            if(pPrefabData != nullptr)
            {
                const YAML::Node& rootData = *pPrefabData->begin();  
                prefabReflectJson = rootData["ReflectFields"].as<std::string>();
                prefabDoc         = yyjson_read(prefabReflectJson.c_str(), prefabReflectJson.size(), 0);
                prefabRoot        = yyjson_doc_get_root(prefabDoc);

                thisReflectJson = SerializedReflectFields();
                thisDoc         = yyjson_read(thisReflectJson.c_str(), thisReflectJson.size(), 0);
                thisRoot        = yyjson_doc_get_root(thisDoc);
            }

            ImGui::Text("Prefab");
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));   
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
            static std::string guidPath;
            guidPath = _prefabGuid.ToPath().string();
            if (guidPath.empty() == false)
            {
                ImGui::InputText("Prefab", &guidPath, ImGuiInputTextFlags_ReadOnly);
            }
            else
            {
                static std::string emptyPath = STR_NULL;
                ImGui::InputText("Prefab", &emptyPath, ImGuiInputTextFlags_ReadOnly);
            }     
            ImGui::PopStyleColor(2);
            if(ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
                {
                    DragDropAsset::Data* data = (DragDropAsset::Data*)payload->Data;
                    if (data->pContext->expired() == false)
                    {
                        auto                  context   = data->pContext->lock();
                        const File::Path&     path      = context->GetPath();
                        std::filesystem::path extension = path.extension();
                        if (extension == UmGameObjectFactory.PREFAB_EXTENSION)
                        {
                            UmGameObjectFactory.UnpackPrefab(this);
                            UmGameObjectFactory.PackPrefab(this, context->GetMeta().GetFileGuid());
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::Separator();
        }

        if (isPrefab)
        {
            //??????????????
            UmCore->ImGuiDrawPropertysSetting.InputEndEvent = [&](bool result, std::string_view) {
                if (true == result && true == isPrefab)
                {
                    auto testFunc = [&](std::string_view name, void* pValue) 
                    {
                        bool isOverrideField = UmGameObjectFactory.IsOverrideField(pValue);
                        if (false == isOverrideField)
                        {
                            yyjson_val* prefabVal    = yyjson_obj_get(prefabRoot, name.data());
                            std::string prefabValStr = ReflectHelper::json::yyjsonValToString(prefabVal);

                            yyjson_val* thisVal    = yyjson_obj_get(thisRoot, name.data());
                            std::string thisValStr = ReflectHelper::json::yyjsonValToString(thisVal);

                            if (prefabValStr != thisValStr)
                            {
                                UmGameObjectFactory.SetOverrideFlag(pValue);
                            }
                        }
                    };
                }
            };
        }
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
                const char* className = component->ClassName();
                ImVec2 textSize  = ImGui::CalcTextSize(className);  
                ImVec2 cursorPos = ImGui::GetCursorScreenPos();
                ImVec2 padding = ImVec2(4.0f, 2.0f);
                ImVec2 bgMin = cursorPos;
                ImVec2 bgMax = ImVec2(cursorPos.x + textSize.x + padding.x, cursorPos.y + textSize.y + padding.y);
                ImGui::GetWindowDrawList()->AddRectFilled(bgMin, bgMax, ImGui::GetColorU32(ImVec4(0.2f, 0.204f, 0.212f, 1.0f)));
                ImGui::SetCursorScreenPos(ImVec2(cursorPos.x , cursorPos.y));
                ImGui::Text(className);
                ImGui::SameLine();
                ImGui::Text(" Component");

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

        if (prefabDoc != nullptr)
        {
            yyjson_doc_free(prefabDoc);
        }
        if (thisDoc != nullptr)
        {
            yyjson_doc_free(thisDoc);
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