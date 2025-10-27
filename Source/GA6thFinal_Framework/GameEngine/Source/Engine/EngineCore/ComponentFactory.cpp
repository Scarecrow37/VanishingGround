#include "pch.h"
#include "Mesh/MeshComponent.h"
#include "Camera/CameraComponent.h"

using namespace Global;
using namespace u8_literals;

#define SAFE_FREE(ptr) if(ptr != nullptr) free(ptr)

EComponentFactory::EComponentFactory()
{
   
}
EComponentFactory::~EComponentFactory() = default;

bool EComponentFactory::InitalizeComponentFactory()
{  
    if constexpr (Application::IsEditor())
    {
        if (true == editorModule->PlayMode.IsPlay())
        {
            engineCore->Logger.Log(LogLevel::LEVEL_WARNING, "You cannot build while in Play Mode.");
            return false;
        }

        DWORD exitCodeOut{};
        if (!dllUtility::RunBatchFile(Engine::BUILD_BATCH_PATH, &exitCodeOut))
        {
            engineCore->Logger.Log(LogLevel::LEVEL_ERROR, "Scripts build Fail!");
            return false;
        }
    }
    else
    {
        if (_scriptsDll != NULL)
            return false;
    }

    using AddListTupleType = std::tuple<GameObject*, std::string, int, std::string>;
    static std::vector<AddListTupleType> addList; // 복구해야할 컴포넌트 항목들
    addList.clear();

    SetForegroundWindow(UmApplication.GetHwnd());
    if (_scriptsDll != NULL)
    {
        //커맨드 Clear
        UmCommandManager.Clear();
        //오디오 Clear
        UmAudio.ClearVoicePool();

        //Input Receiver Clear
        ESceneManager::Engine::GetInputSystem().CleanupInputReceivers();


        //모든 컴포넌트 자원 회수
        for (auto& [key, wptr] : _componentInstanceVec)
        {
            if (auto component = wptr.lock())
            {
                int index = component->GetIndex();
                addList.emplace_back(component->_gameObject, key, index, component->SerializedReflectFields());
                component->_gameObject->_components[index].reset(); //컴포넌트 파괴
            }
        }
        _componentInstanceVec.clear();

        // Script Dll에서 생성된 Graphics 자원 회수
        UmGraphics.ClearGraphicsResource();
        FreeLibrary(_scriptsDll);
        _scriptsDll = NULL;
    }

    //인스턴스 아이디에 따른 정렬
    if (1 < addList.size())
    {
        std::ranges::sort(addList, [](AddListTupleType& tupleA, AddListTupleType& tupleB) 
        {
            auto& [objectA, keyA, indexA, dataA] = tupleA;
            auto& [objectB, keyB, indexB, dataB] = tupleB;
            int instanceIDA                      = objectA->GetInstanceID();
            int instanceIDB                      = objectB->GetInstanceID();
            if (instanceIDA != instanceIDB)
            {
                return instanceIDA < instanceIDB;
            }
            else
            {
                return indexA < indexB;
            }
        });
    }

    _newScriptsFunctionMap.clear();
    _newScriptsKeyVec.clear();

    if constexpr (true == Application::IsEditor())
    {
        SetDllDirectory(EComponentFactory::Engine::SCRIPTS_DLL_PATH);
    }
    _scriptsDll = LoadLibraryW(L"GameScripts.dll");
    if (_scriptsDll == NULL)
    {
        //DLL Load Fail
        __debugbreak();
        return false;
    }

    //스크립트 파일 생성 함수 등록
    std::vector<std::string> funcList = dllUtility::GetDLLFuntionNameList(_scriptsDll);
    MakeScriptFunc = (MakeUmScriptsFile)GetProcAddress(_scriptsDll, funcList[0].c_str());
    if (funcList[0] != "CreateUmrealcSriptFile")
    {
        FreeLibrary(_scriptsDll);
        _scriptsDll = NULL;
        __debugbreak(); //초기화 함수 에러.
        return false;
    }

    //스크립트 초기화 함수 등록
    if (funcList[1] != "InitalizeUmrealScript")
    {
        FreeLibrary(_scriptsDll);
        _scriptsDll = NULL;
        __debugbreak(); //초기화 함수 에러.
        return false;
    }
    auto InitDLLCores = (InitScripts)GetProcAddress(_scriptsDll, funcList[1].c_str());
    std::shared_ptr<EngineCores> cores = engineCore;
    InitDLLCores(
        cores,
        ImGui::GetCurrentContext());

    //스크립트 생성자들 등록
    AddEngineComponentsToScripts();
    for (size_t i = 0; i < funcList.size(); i++)
    {
        std::string& funcName = funcList[i];
        if (funcName.find("New") != std::string::npos)
        {
            auto NewComponentFunc = (NewScripts)GetProcAddress(_scriptsDll, funcName.c_str());
            Component* component = NewComponentFunc();
            const char* key = typeid(*component).name();
            _newScriptsFunctionMap[key] = NewComponentFunc;
            _newScriptsKeyVec.emplace_back(key);
            delete component;
        }
    }
    std::sort(_newScriptsKeyVec.begin(), _newScriptsKeyVec.end());

    //파괴된 컴포넌트 재생성 및 복구
    MissingComponent missingTemp;
    for (auto& [gameObject, key, index, reflectData] : addList)
    {
        bool isMissing = false;
        if (key == typeid(MissingComponent).name())
        {
            //Missing 컴포넌트면 데이터 복구
            missingTemp.DeserializedReflectFields(reflectData);
            key = missingTemp.ReflectFields->typeName;
            isMissing = true;
        }
        std::shared_ptr<Component> newComponent;
        auto findIter = _newScriptsFunctionMap.find(key);
        bool isFind   = findIter != _newScriptsFunctionMap.end();
        if (isFind)
        {
            //컴포넌트 존재하면 다시 생성
            newComponent = NewComponent(key);
        }
        else 
        {
            //없어진 컴포넌트면 Missing으로 대체
            std::shared_ptr<MissingComponent> missing = NewMissingComponent();
            if (false == isMissing)
            {
                missing->ReflectFields->typeName    = key;
                missing->ReflectFields->reflectData = reflectData;
            }
            else
            {
                missing->ReflectFields->typeName    = missingTemp.ReflectFields->typeName;
                missing->ReflectFields->reflectData = missingTemp.ReflectFields->reflectData;
            }
            newComponent = std::move(missing);
        }
        ResetComponent(gameObject, newComponent);       // 엔진에서 사용하기 위해 초기화
        newComponent->_initFlags.SetAwake();            // 초기화 플래그 설정
        newComponent->_initFlags.SetStart();            // 초기화 플래그 설정
        gameObject->_components[index] = newComponent;  
        if (isFind == true)
        {
            if (isMissing == true)
            {
                //Missing 컴포넌트면 데이터 복구
                reflectData = missingTemp.ReflectFields->reflectData;
            }
            if (reflectData.empty() == false)
            {
                newComponent->DeserializedReflectFields(reflectData); // 데이터 복구
                if (gameObject->IsValid())
                {
                    newComponent->Reset();
                }
            }          
            newComponent->UpdateEnableInHierarchy();
        }     
    }
   
    //존재 안하는거는 전부 제거 및 Added 호출
    for (auto& [gameObject, key, index, reflectData] : addList)
    {
        std::erase_if(gameObject->_components, [](auto& sptr)
        {
            bool isErase = sptr == nullptr;
            if (false == isErase)
            {
                sptr->Added();
            }
            return isErase;
        });
    }
    return true;
}

void EComponentFactory::UninitalizeComponentFactory()
{
    if (_scriptsDll != NULL)
    {
        //모든 컴포넌트 자원 회수
        for (auto& [key, wptr] : _componentInstanceVec)
        {
            if (auto component = wptr.lock())
            {
                int index = component->GetIndex();
                if (0 <= index)
                {
                    component->_gameObject->_components[index].reset(); // 컴포넌트 파괴
                }             
            }
        }
        _componentInstanceVec.clear();

        // Input Receiver Clear
        ESceneManager::Engine::GetInputSystem().CleanupInputReceivers();

        // 오디오 Clear
        UmAudio.ClearVoicePool();

        // Script Dll에서 생성된 Graphics 자원 회수
        UmGraphics.ClearGraphicsResource();
        FreeLibrary(_scriptsDll);
        _scriptsDll = NULL;
    }
}

Component* EComponentFactory::AddComponentToObject(GameObject* ownerObject, std::string_view typeid_name)
{
    if(std::shared_ptr<Component> sptr_component = NewComponent(typeid_name))
    {
        const char* name = typeid_name.data();
        ResetComponent(ownerObject, sptr_component);
        ESceneManager::Engine::AddComponentToLifeCycle(sptr_component); //씬에 등록
        return sptr_component.get();
    }
    return nullptr;
}

void EComponentFactory::MakeScriptFile(const char* fileName) const
{
    if (_scriptsDll != NULL)
    {
        MakeScriptFunc(fileName);
    }
    else
    {
        engineCore->Logger.Log(LogLevel::LEVEL_WARNING, u8"Script DLL을 빌드해주세요!"_c_str);
    }
}

YAML::Node EComponentFactory::SerializeToYaml(Component* component)
{
    if (UmComponentFactory.HasScript() == false)
    {
        if (UmComponentFactory.InitalizeComponentFactory() == false)
        {
            UmLogger.Log(LogLevel::LEVEL_FATAL, u8"스크립트 빌드 에러 해결 필요."_c_str);
            __debugbreak();
            UmApplication.Quit();
            return YAML::Node();
        }
    }
    return MakeYamlToComponent(component);
}

bool EComponentFactory::ParsingYamlToOverrideFlags(Component* component, const YAML::Node& componentNode) 
{
    if (nullptr == component)
    {
        __debugbreak();
    }

    bool result = false;
    int SerializedVersion = 0;
    const YAML::Node& node = componentNode;
    if (node["SerializedVersion"])
    {
        SerializedVersion = node["SerializedVersion"].as<int>();
    }

    if (0 < SerializedVersion)
    {
        if constexpr (Application::IsEditor())
        {
            const char* componentType = typeid(*component).name();
            std::string nodeType = componentNode["Type"].as<std::string>();
            if (nodeType == componentType)
            {                     
                if (node["OverrideFlags"])
                {
                    const YAML::Node& overrideFlagsNode = node["OverrideFlags"];
                    if (false == overrideFlagsNode.IsNull())
                    {
                        std::string propertyName;
                        std::vector<std::string> overrideFieldNames;
                        component->applyReflectFields([&](std::string_view name, void* pData) 
                        {
                            if (overrideFlagsNode[name.data()])
                            {
                                propertyName = overrideFlagsNode[name.data()].as<std::string>();
                                UmGameObjectFactory.SetOverrideFlag(pData, propertyName);
                                overrideFieldNames.emplace_back(name.data());
                            }
                        });

                        if (false == overrideFieldNames.empty())
                        {
                            using namespace ReflectHelper::json;

                            std::string prefabData = component->SerializedReflectFields();
                            yyjson_doc* prefabDoc  = yyjson_read(prefabData.c_str(), prefabData.size(), 0);
                            yyjson_mut_doc* prefabMutDoc = yyjson_doc_mut_copy(prefabDoc, nullptr);
                            yyjson_mut_val* prefabRoot = yyjson_mut_doc_get_root(prefabMutDoc);

                            std::string myData = componentNode["ReflectFields"].as<std::string>();
                            yyjson_doc* myDoc  = yyjson_read(myData.c_str(), myData.size(), 0);
                            yyjson_val* myRoot = yyjson_doc_get_root(myDoc);

                            bool isWrite = false;
                            for (auto& name : overrideFieldNames)
                            {
                                yyjson_val* myVal = yyjson_obj_get(myRoot, name.data());
                                if (myVal)
                                {
                                    yyjson_mut_val* prefabVal = yyjson_mut_obj_get(prefabRoot, name.data());
                                    if (prefabVal)
                                    {
                                        yyjson_mut_val* prefabKey = yyjson_mut_strcpy(prefabMutDoc, name.data());
                                        yyjson_mut_val* prefabVal = yyjson_val_mut_copy(prefabMutDoc, myVal);
                                        yyjson_mut_obj_replace(prefabRoot, prefabKey, prefabVal);

                                        isWrite = true;
                                    }
                                }
                            }
                            
                            if (isWrite)
                            {
                                char* str = yyjson_mut_write(prefabMutDoc, 0, nullptr);
                                if (str)
                                {
                                    component->DeserializedReflectFields(str);
                                    SAFE_FREE(str);
                                }
                            }

                            yyjson_doc_free(prefabDoc);
                            yyjson_mut_doc_free(prefabMutDoc);
                            yyjson_doc_free(myDoc);
                        }
                    }
                }
                result = true;
            }
        }
    }
    return result;
}

Component* EComponentFactory::AddComponentToYamlLifeCycle(GameObject* ownerObject, YAML::Node* componentNode)
{
    if (UmComponentFactory.HasScript() == false)
    {
        if (UmComponentFactory.InitalizeComponentFactory() == false)
        {
            UmLogger.Log(LogLevel::LEVEL_FATAL, u8"스크립트 빌드 에러 해결 필요."_c_str);
            __debugbreak();
            UmApplication.Quit();
            return nullptr;
        }
    }
    std::shared_ptr<Component> component;
    if (component = MakeComponentToYaml(ownerObject, componentNode))
    {
        ESceneManager::Engine::AddComponentToLifeCycle(component); // 씬에 등록
    }
    else
    {
        return nullptr;
    }
    return component.get();
}

Component* EComponentFactory::AddComponentToYamlNow(GameObject* ownerObject, YAML::Node* componentNode)
{
    if (UmComponentFactory.HasScript() == false)
    {
        if (UmComponentFactory.InitalizeComponentFactory() == false)
        {
            UmLogger.Log(LogLevel::LEVEL_FATAL, u8"스크립트 빌드 에러 해결 필요."_c_str);
            __debugbreak();
            UmApplication.Quit();
            return nullptr;
        }
    }
    std::shared_ptr<Component> component;
    if (component = MakeComponentToYaml(ownerObject, componentNode))
    {
        PushBackComponentToObject(component);   
    }
    else
    {
        return nullptr;
    }
    return component.get();
}

void EComponentFactory::Engine::PushBackComponentToObject(std::shared_ptr<Component>& component) 
{
    UmComponentFactory.PushBackComponentToObject(component);
}

void EComponentFactory::PushBackComponentToObject(std::shared_ptr<Component>& component)
{
    if (component->_gameObject)
    {
        component->_gameObject->_components.emplace_back(component);
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, u8"초기화 되지 않은 컴포넌트 입니다.");
    }
}

void EComponentFactory::InsertComponentToObject(GameObject* object, std::shared_ptr<Component>& component, int index) 
{
    object->_components.insert(object->_components.begin() + index, component);
}

bool EComponentFactory::RevertOverrideField(Component* component, std::string_view fieldName)
{
    bool result = false;
    if constexpr (Application::IsEditor())
    {
        EGameObjectFactory& gameObjectFactory = UmGameObjectFactory;
        GameObject* prefabInstance = component->gameObject->PrefabInstance;
        if (nullptr != prefabInstance)
        {
            const std::vector<std::shared_ptr<GameObject>>* prefabList = gameObjectFactory.GetOriginPrefab(prefabInstance->_prefabGuid);
            if (nullptr != prefabList)
            {
                int prefabIndex = -1;
                int currIndex = 0;
                Transform::ForeachBFS(prefabInstance->_transform, [&](Transform* curr) 
                {
                    if (&component->gameObject == &curr->gameObject)
                    {
                        prefabIndex = currIndex;
                    }
                    currIndex++;
                });

                if (0 <= prefabIndex)
                {
                    GameObject* prefab = (*prefabList)[prefabIndex].get();
                    Component* prefabComponent = prefab->GetComponentAtIndex<Component>(component->GetIndex());
                    if (typeid(*component) == typeid(*prefabComponent))
                    {
                        using namespace ReflectHelper::json;
                        std::string prefabData = prefabComponent->SerializedReflectFields();
                        yyjson_doc* prefabDoc = yyjson_read(prefabData.c_str(), prefabData.size(), 0);
                        yyjson_val* prefabRoot = yyjson_doc_get_root(prefabDoc);

                        std::string myData = component->SerializedReflectFields();
                        yyjson_doc* myDoc = yyjson_read(myData.c_str(), myData.size(), 0);
                        yyjson_mut_doc* myMutDoc = yyjson_doc_mut_copy(myDoc, nullptr);
                        yyjson_mut_val* myRoot = yyjson_mut_doc_get_root(myMutDoc);

                        bool isWrite = false;
                        yyjson_val* prefabVal = yyjson_obj_get(prefabRoot, fieldName.data());
                        if (prefabVal)
                        {
                            yyjson_mut_val* myVal = yyjson_mut_obj_get(myRoot, fieldName.data());
                            if (myVal)
                            {
                                yyjson_mut_val* myKey = yyjson_mut_strcpy(myMutDoc, fieldName.data());
                                yyjson_mut_val* myVal = yyjson_val_mut_copy(myMutDoc, prefabVal);
                                yyjson_mut_obj_replace(myRoot, myKey, myVal);
                                isWrite = true;
                            }
                        }
                        
                        if (isWrite)
                        {
                            char* str = yyjson_mut_write(myMutDoc, 0, nullptr);
                            if (str)
                            {
                                component->DeserializedReflectFields(str);
                                SAFE_FREE(str);
                            }
                        }

                        yyjson_doc_free(prefabDoc);
                        yyjson_doc_free(myDoc);
                        yyjson_mut_doc_free(myMutDoc);

                        result = true;
                    }
                }
            }
        }
    }

    if (result == false)
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, u8"프리팹 구조와 오브젝트 구조가 다릅니다."_c_str);
    }
    return result;
}

void EComponentFactory::AddEngineComponentsToScripts() 
{
    for (auto& [key, func] : _engineComponets)
    {
        _newScriptsFunctionMap[key] = func;
        _newScriptsKeyVec.emplace_back(key);
    }
}

std::shared_ptr<Component> EComponentFactory::NewComponent(std::string_view typeid_name)
{
    std::shared_ptr<Component> newComponent;
    auto findIter = _newScriptsFunctionMap.find(typeid_name.data());
    if (findIter != _newScriptsFunctionMap.end())
    {
        auto& [name, func] = *findIter;
        newComponent.reset(func());                                //컴포넌트 생성
        _componentInstanceVec.emplace_back(name, newComponent);   //추적용 weak_ptr 생성 
    }
    return newComponent;
}

std::shared_ptr<MissingComponent> EComponentFactory::NewMissingComponent()
{
    std::shared_ptr<MissingComponent> missing = std::make_shared<MissingComponent>();
    _componentInstanceVec.emplace_back(typeid(MissingComponent).name(), missing);
    return missing;
}

void EComponentFactory::ResetComponent(GameObject* ownerObject, std::shared_ptr<Component>& component)
{
    //여긴 엔진에서 사용하기 위한 초기화 코드 
    component->_className = (typeid(*component).name() + 6);
    component->_gameObject = ownerObject;
    component->_weakPtr = component;
    if (Component::TYPE::MESH == component->GetType())
    {
        MeshComponent* meshComponent = static_cast<MeshComponent*>(component.get());
        ESceneManager::Engine::PushRuntimeMeshComponent(meshComponent);
    }
    else if (component->_type == Component::TYPE::CAMERA)
    {
        CameraComponent* camera = static_cast<CameraComponent*>(component.get());
        std::shared_ptr<Camera> newCamera(new Camera);
        camera->SetTarget(newCamera);
    }
    //end
}

YAML::Node EComponentFactory::MakeYamlToComponent(Component* component)
{
    constexpr int SerializedVersion = 1;
    YAML::Node node;
    if constexpr (0 < SerializedVersion)
    {
        node["SerializedVersion"] = SerializedVersion;
    }
    node["Type"] = typeid(*component).name();
    node["ReflectFields"] = component->SerializedReflectFields();
    if constexpr (0 < SerializedVersion)
    {
        if constexpr (Application::IsEditor())
        {
            YAML::Node overrideFlagsNode;
            std::string_view propertyName;
            component->applyReflectFields([&](std::string_view name, void* pData) 
            {
                if (true == UmGameObjectFactory.IsOverrideField(pData, &propertyName))
                {
                    overrideFlagsNode[name.data()] = propertyName;
                }
            });
            if (false == overrideFlagsNode.IsNull())
            {
                node["OverrideFlags"] = overrideFlagsNode;
            }
        }
    }
    return node;
}

std::shared_ptr<Component> EComponentFactory::MakeComponentToYaml(GameObject* ownerObject, YAML::Node* pComponentNode)
{
    int SerializedVersion = 0;
    YAML::Node& node = *pComponentNode;
    if (node["SerializedVersion"])
    {
        SerializedVersion = node["SerializedVersion"].as<int>();
    }
    std::string Type = node["Type"].as<std::string>();
    std::string ReflectFields = node["ReflectFields"].as<std::string>();

    // YAML 노드에 명시된 타입으로 컴포넌트 생성을 시도합니다.
    std::shared_ptr<Component> component = NewComponent(Type);
    if (component)
    {
        // 생성에 성공하면 필드를 역직렬화합니다.
        ResetComponent(ownerObject, component);
        component->DeserializedReflectFields(ReflectFields);
    }
    else
    {
        // 생성에 실패한 경우, 해당 컴포넌트는 삭제되었거나 이름이 변경되었을 수 있습니다.
        // MissingComponent를 사용해 원본 데이터를 파싱하고 원래의 타입 이름을 찾습니다.
        auto missing = NewMissingComponent();
        missing->DeserializedReflectFields(ReflectFields);
        const std::string& originalType = missing->ReflectFields->typeName;
        const std::string& originReflectFields = missing->ReflectFields->reflectData;

        // 복구된 타입 이름으로 컴포넌트 생성을 다시 시도합니다.
        component = NewComponent(originalType);
        if (component)
        {
            ResetComponent(ownerObject, component);
            component->DeserializedReflectFields(originReflectFields);
        }
        else
        {
            if (Type != typeid(MissingComponent).name())
            {
                missing->ReflectFields->typeName    = Type;
                missing->ReflectFields->reflectData = ReflectFields;
            }
            component = std::move(missing);
            ResetComponent(ownerObject, component);
        }
    }  
    return component;
}
