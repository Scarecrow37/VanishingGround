#include "pch.h"
using namespace Global;
using namespace u8_literals;

void EGameObjectFactory::RegisterGameObjects()
{
    //엔진에 존재하는 모든 오브젝트 타입들을 등록해야합니다.
    RegisterGameObject<GameObject>();
}

void EGameObjectFactory::Engine::RegisterFileEvents()
{
    UmFileSystem.RegisterFileEventSubscriber(&UmGameObjectFactory, {EGameObjectFactory::PREFAB_EXTENSION});
}

void EGameObjectFactory::WritePrefabGuid(const File::Path& path, YAML::Node& data) 
{
    YAML::Node& prefabNode = data;
    YAML::Node  rootNode   = *prefabNode.begin();
    File::Guid  prefabGuid = rootNode["Prefab"].as<std::string>();
    if (prefabGuid != path.ToGuid())
    {
        rootNode["Prefab"] = path.ToGuid().string();
        std::ofstream ofs(path, std::ios::trunc);
        if (ofs.is_open())
        {
            ofs << prefabNode;
        }
        ofs.close();
    }

    auto& guidQueue = _prefabGuidQueue[path];
    if (guidQueue.empty() == false)
    {
        for (auto& weakObject : guidQueue)
        {
            auto pObject = weakObject.lock();
            if (pObject != nullptr)
            {
                if (pObject->IsPrefabInstance() == true)
                {
                    UnpackPrefab(pObject.get());
                }
                PackPrefab(pObject.get(), path.ToGuid());
            }
        }
        guidQueue.clear();
    }
}

void EGameObjectFactory::ApplyPrefabInstanceChanges(const File::Guid& guid, YAML::Node& yaml) 
{
    auto findIter = _prefabInstanceList.find(guid);
    if (findIter != _prefabInstanceList.end())
    {
        auto& [guid, list] = *findIter;
        std::erase_if(list, [](auto& waek) 
        { 
            return waek.expired();
        });

        if (false == list.empty())
        {     
            std::vector<std::shared_ptr<GameObject>> instanceList;
            for (auto& wptr : list)
            {
                if (false == wptr.expired())
                {
                    instanceList.push_back(wptr.lock());
                }
            }
            for (auto& gameObject : instanceList)
            {
                YAML::Node myYaml = SerializeToYaml(gameObject.get());
                auto prefabObjects = MakeObjectsGraphToYaml(&yaml, true, &myYaml);
                if (false == prefabObjects.empty())
                {
                    int i = 0;
                    Transform::ForeachBFS(gameObject->_transform, [&](Transform* curr) 
                    {
                        if (i < prefabObjects.size())
                        {
                            ESceneManager::Engine::SwapPrefabInstance(&curr->gameObject, prefabObjects[i].get());
                            i++;
                        }
                        else
                        {
                            GameObject::Destroy(curr->gameObject);
                        }
                    });

                    if (i < prefabObjects.size())
                    {
                        std::string_view ownerScene = prefabObjects[i - 1]->_ownerScene;
                        for (; i < prefabObjects.size(); i++)
                        {
                            auto& curr        = prefabObjects[i];
                            curr->_ownerScene = ownerScene;
                            curr->_instanceID = InstanceID.CreateInstanceID();
                            ESceneManager::Engine::AddGameObjectToLifeCycle(curr);
                        }
                    }
                    auto prefabRoot = prefabObjects.front().get();
                    UmGameObjectFactory.UnpackPrefab(prefabRoot);
                    UmGameObjectFactory.PackPrefab(prefabRoot, guid);
                }
            }
        }
    }
}

void EGameObjectFactory::OnFileRegistered(const File::Path& path)
{
    File::Guid guid = path.ToGuid();
    YAML::Node yamlData = YAML::LoadFile(path.string());
    _prefabObjectMap[guid] = MakeObjectsGraphToYaml(&yamlData, true);
    WritePrefabGuid(path, yamlData);
    ApplyPrefabInstanceChanges(guid, yamlData);
}

void EGameObjectFactory::OnFileUnregistered(const File::Path& path) 
{
    File::Guid guid = path.ToGuid();
    _prefabObjectMap.erase(guid);
}

void EGameObjectFactory::OnFileModified(const File::Path& path)
{
    File::Guid guid = path.ToGuid();
    YAML::Node yamlData = YAML::LoadFile(path.string());
    _prefabObjectMap[guid] = MakeObjectsGraphToYaml(&yamlData, true);
    WritePrefabGuid(path, yamlData);
    ApplyPrefabInstanceChanges(guid, yamlData);
}

void EGameObjectFactory::OnFileRemoved(const File::Path& path) 
{
    File::Guid guid = path.ToGuid();
    _prefabObjectMap.erase(guid);
}

void EGameObjectFactory::OnFileRenamed(const File::Path& oldPath, const File::Path& newPath) 
{

}

void EGameObjectFactory::OnFileMoved(const File::Path& oldPath, const File::Path& newPath) 
{

}

EGameObjectFactory::EGameObjectFactory()
{
    RegisterGameObjects();
}

EGameObjectFactory::~EGameObjectFactory() = default;

std::shared_ptr<GameObject> EGameObjectFactory::NewGameObject(std::string_view typeid_name, std::string_view name)
{
    auto sptr_object = MakeGameObject(typeid_name);
    if (sptr_object != nullptr)
    {
        ResetGameObject(sptr_object.get(), name);
        ESceneManager::Engine::AddGameObjectToLifeCycle(sptr_object);
    }
    return sptr_object;
}

YAML::Node EGameObjectFactory::SerializeToYaml(GameObject* gameObject, bool onlyVaildObject)
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

    YAML::Node nodes;
    std::map<Transform*, int> transformParentLevelMap;
    int parentIndex = 0;
    bool isPrefabInstance = gameObject->IsPrefabInstance();
    Transform::ForeachBFS(
    gameObject->_transform, 
    [&](Transform* curr) 
    {
        if (false == onlyVaildObject || true == curr->gameObject->IsValid())
        {
            // 오브젝트 직렬화
            YAML::Node objectNode = MakeYamlToGameObject(&curr->gameObject);

            // 컴포넌트들 직렬화
            for (auto& component : curr->gameObject->_components)
            {
                YAML::Node componentNode = UmComponentFactory.SerializeToYaml(component.get());
                objectNode["Components"].push_back(componentNode);
            }

            // Transform 직렬화
            transformParentLevelMap[curr]   = parentIndex;
            YAML::Node transformNode        = objectNode["Transform"].as<YAML::Node>();
            transformNode["TransformIndex"] = parentIndex;
            if (curr->Parent != nullptr)
            {
                transformNode["ParentIndex"] = transformParentLevelMap[curr->Parent];
            }
            ++parentIndex;
            nodes.push_back(objectNode);
        }

    });
    return nodes;
}

std::vector<std::shared_ptr<GameObject>> EGameObjectFactory::MakeObjectsGraphToYaml(YAML::Node* pObjectNode, bool useResource, YAML::Node* pSceneObjectNode) 
{
    std::vector<std::shared_ptr<GameObject>> makeList;
    if (UmComponentFactory.HasScript() == false)
    {
        if (UmComponentFactory.InitalizeComponentFactory() == false)
        {
            UmLogger.Log(LogLevel::LEVEL_FATAL, u8"스크립트 빌드 에러 해결 필요."_c_str);
            __debugbreak(); 
            UmApplication.Quit();
            return makeList;
        }
    }

    YAML::Node& nodes = *pObjectNode;
    YAML::const_iterator sceneNodes;
    if (nullptr != pSceneObjectNode)
    {
        sceneNodes = pSceneObjectNode->begin();
    }

    std::map<int, Transform*> transformParentLevelMap;
    std::shared_ptr<GameObject> currObject;
    bool isPrefabInstance = false;
    for (auto node : nodes)
    {
        // 오브젝트 생성
        YAML::Node& currNode = node;
        std::string Type = currNode["Type"].as<std::string>();
        std::shared_ptr<GameObject> currObject = MakeGameObject(Type);
        if (useResource == false)
        {
            ResetGameObject(currObject.get(), "null");
        }
        ParsingYamlToGameObject(currObject.get(), currNode);

        // 프리팹 추적
        if (currNode["Prefab"])
        {
            File::Guid prefab = currNode["Prefab"].as<std::string>();
            if (prefab != STR_NULL)
            {              
                if (useResource == false)
                {
                    std::vector<std::weak_ptr<GameObject>>& instanceList = _prefabInstanceList[prefab];
                    instanceList.emplace_back(currObject);
                    if (nullptr != pSceneObjectNode)
                    {
                        const YAML::Node& currSceneNodes = *sceneNodes;
                        ParsingYamlToGameObject(currObject.get(), currSceneNodes);
                    }
                }
                else
                {
                    currObject->_prefabGuid = prefab;
                }
                isPrefabInstance = true;
            }
        }

        // 컴포넌트들 역직렬화
        if (currNode["Components"])
        {
            YAML::Node componentNodes = currNode["Components"];
            YAML::const_iterator sceneComponentNodeIter;
            if (true == isPrefabInstance)
            {
                const YAML::Node& currSceneNodes = *sceneNodes; 
                sceneComponentNodeIter = currSceneNodes["Components"].begin();
            }

            for (auto componentNode : componentNodes)
            {
                Component* component = nullptr;
                YAML::Node& currComponentNode = componentNode;
                if (useResource == false)
                {
                    component = UmComponentFactory.AddComponentToYamlLifeCycle(currObject.get(), &currComponentNode);
                }
                else
                {
                    component = UmComponentFactory.AddComponentToYamlNow(currObject.get(), &currComponentNode);
                }

                if (true == isPrefabInstance)
                {
                    const YAML::Node& currSceneNodes = *sceneNodes; 
                    if (sceneComponentNodeIter != currSceneNodes["Components"].end())
                    {
                        bool result = UmComponentFactory.ParsingYamlToOverrideFlags(component, *sceneComponentNodeIter);
                        if (true == result)
                        {
                            ++sceneComponentNodeIter;
                        }
                    }
                }
            }
        }

        // Transform 역직렬화
        YAML::Node transformNode  = currNode["Transform"].as<YAML::Node>();
        int        TransformIndex = transformNode["TransformIndex"].as<int>();
        transformParentLevelMap[TransformIndex] = &currObject->_transform;
        if (transformNode["ParentIndex"])
        {
            int        ParentIndex = transformNode["ParentIndex"].as<int>();
            Transform* pParent     = transformParentLevelMap[ParentIndex];
            currObject->_transform.SetParent(pParent, false);
        }

        makeList.push_back(currObject);
        if (nullptr != pSceneObjectNode)
        {
           ++sceneNodes;
        }
    }

    //리소스는 Active 비활성화
    if (true == useResource)
    {
        auto& root = makeList.front();
        root->ReflectFields->_activeSelf = false;
        GameObject::Engine::UpdateActiveInHierarchy(root.get());
    }

    //게임 오브젝트의 _activeInHierarchy 계산
    for (auto& object : makeList)
    {
        GameObject::Engine::ResetActiveInHierarchy(object.get());
    }
    return makeList;
}

std::shared_ptr<GameObject> EGameObjectFactory::DeserializeToYaml(YAML::Node* pObjectNode, YAML::Node* sceneObjectNode)
{
    auto makeList = MakeObjectsGraphToYaml(pObjectNode, false, sceneObjectNode);
    if (makeList.empty())
    {
        return nullptr;
    }

    for (auto& ptr : makeList)
    {
        ESceneManager::Engine::AddGameObjectToLifeCycle(ptr);
    }
    return makeList[0];
}

std::shared_ptr<GameObject> EGameObjectFactory::DeserializeToGuid(const File::Guid& guid, YAML::Node* sceneNode)
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

    auto iter = _prefabObjectMap.find(guid);
    if (iter == _prefabObjectMap.end())
    {
        auto tempObject = NewGameObject(typeid(GameObject).name(), "GameObject");
        std::vector<std::weak_ptr<GameObject>>& instanceList = _prefabInstanceList[guid];
        instanceList.emplace_back(tempObject);
        tempObject->_prefabGuid = guid;
        return tempObject;
    }
    YAML::Node yamlData = SerializeToYaml(iter->second[0].get());
    auto pObject = DeserializeToYaml(&yamlData, sceneNode);
    return pObject;
}

std::shared_ptr<GameObject> EGameObjectFactory::DeserializeToSceneObject(YAML::Node& sceneObjectsNode)
{
    auto yamlIter = sceneObjectsNode.begin();
    YAML::Node rootObjectNode = *yamlIter;
    File::Guid prefabGuid = rootObjectNode["Prefab"].as<std::string>();
    std::shared_ptr<GameObject> newObject;
    if (prefabGuid != STR_NULL)
    {
        newObject = UmGameObjectFactory.DeserializeToGuid(prefabGuid, &sceneObjectsNode);
    }
    else
    {
        newObject = UmGameObjectFactory.DeserializeToYaml(&sceneObjectsNode);
    }
    return newObject;
}

void EGameObjectFactory::WriteGameObjectFile(Transform* transform, std::string_view outPath)
{
    namespace fs     = std::filesystem;
    using fsPath     = std::filesystem::path;
    fsPath writePath = UmFileSystem.GetAssetPath();
    writePath /= outPath;
    writePath /= transform->gameObject->ToString();
    writePath.replace_extension(PREFAB_EXTENSION);
    bool isExists = fs::exists(writePath);
    if (true == isExists)
    {
        int result = MessageBox(UmApplication.GetHwnd(), L"파일이 이미 존재합니다. 덮어쓰겠습니까?",
                                L"파일이 존재합니다.", MB_YESNO);
        if (result != IDYES)
        {
            return;
        }
    }
  
    fs::create_directories(writePath.parent_path());
    YAML::Node node = UmGameObjectFactory.SerializeToYaml(&transform->gameObject);
    if (node.IsNull() == false)
    {
        std::ofstream ofs(writePath, std::ios::trunc);
        if (ofs.is_open())
        {
            ofs << node;
        }
        ofs.close();

        GameObject& object = transform->gameObject;
        if (false == isExists)
        {
            _prefabGuidQueue[writePath].emplace_back(object.GetWeakPtr());
        }
        else
        {
            if (object.IsPrefabInstance() == true)
            {
                UnpackPrefab(&object);
            }
            File::Path path(writePath);
            PackPrefab(&object, path.ToGuid());
        }
    }  
}

bool EGameObjectFactory::PackPrefab(GameObject* targetObject, const File::Guid& guid)
{
    if (targetObject->IsPrefabInstance() == false)
    {
        if (_prefabObjectMap.find(guid) != _prefabObjectMap.end())
        {
            if (_prefabInstanceList[guid].empty() == false)
            {
                std::erase_if(_prefabInstanceList[guid], 
                [targetObject](std::weak_ptr<GameObject>& weakObject) 
                {
                    return weakObject.expired();
                });
            }                              
        }
        _prefabInstanceList[guid].emplace_back(targetObject->GetWeakPtr());
        targetObject->_prefabGuid = guid;
        return true;
    }
    return false;
}

const std::vector<std::shared_ptr<GameObject>>* EGameObjectFactory::GetOriginPrefab(const File::Guid& guid)
{
    auto findIter = _prefabObjectMap.find(guid);
    if (findIter != _prefabObjectMap.end())
    {
        return &findIter->second;
    }
    return nullptr;
}

bool EGameObjectFactory::UnpackPrefab(GameObject* targetObject)
{
    if (targetObject->IsPrefabInstance() == true)
    {
        auto findIter = _prefabInstanceList.find(targetObject->_prefabGuid);
        if (findIter != _prefabInstanceList.end())
        {
            std::vector<std::weak_ptr<GameObject>>& instanceList = findIter->second;
            std::erase_if(instanceList, 
                [targetObject](std::weak_ptr<GameObject>& weakObject) 
                { 
                    bool isUnpackObject = false;
                    bool isExpired = weakObject.expired();
                    if (isExpired == false)
                    {
                        std::shared_ptr<GameObject> pObject = weakObject.lock();
                        isUnpackObject = targetObject == pObject.get();
                    }
                    return isExpired || isUnpackObject; 
                });
            targetObject->_prefabGuid = STR_NULL;
            return true;
        }
    }
    return false;
}

bool EGameObjectFactory::IsOverrideField(void* pField, std::string_view* outPropertyName)
{
    bool result = false;
    auto findIter = _prefabInstanceOverride.find(pField);
    if (findIter != _prefabInstanceOverride.end())
    {
        result = true;
        if (outPropertyName != nullptr)
        {
            *outPropertyName = findIter->second;
        }
    }
    return result;
}

bool EGameObjectFactory::SetOverrideFlag(void* pField, std::string_view propertyName)
{
    bool result = false;
    auto findIter = _prefabInstanceOverride.find(pField);
    if (findIter == _prefabInstanceOverride.end())
    {
        result = true;
        _prefabInstanceOverride[pField] = propertyName.data();
    }
    return result;
}

bool EGameObjectFactory::UnsetOverrideFlag(void* pField)
{
    bool result = false;
    size_t eraseCount = _prefabInstanceOverride.erase(pField);
    if (eraseCount > 0)
    {
        result = true;
    }
    return result;
}

std::shared_ptr<GameObject> EGameObjectFactory::MakeGameObject(std::string_view typeid_name)
{
    std::shared_ptr<GameObject> newObject;
    auto findIter = _NewGameObjectFuncMap.find(typeid_name.data());
    if (findIter != _NewGameObjectFuncMap.end())
    {
        auto& [key, NewObjectFunc] = *findIter;
        newObject.reset(NewObjectFunc());
        newObject->_weakPtr = newObject;
    }
    else
    {
        std::string message = std::format("{}{}", typeid_name, u8"는 존재하지 않는 오브젝트 타입입니다."_c_str);
        UmLogger.Log(LogLevel::LEVEL_ERROR, message);
    }
    return newObject;
}

void EGameObjectFactory::ResetGameObject(
    GameObject* ownerObject, 
    std::string_view name)
{
    Scene* mainScene = engineCore->SceneManager.GetMainScene();
    if (mainScene != nullptr)
    {
        ownerObject->_ownerScene = mainScene->Path;
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_FATAL, u8"씬이 로드되지 않았습니다."_c_str);
        __debugbreak();
    }   
    ownerObject->ReflectFields->_name = name;
    ownerObject->ReflectFields->_isStatic = false;
    ownerObject->ReflectFields->_activeSelf = true;
  
    //인스턴스 아이디 부여
    int instanceID = InstanceID.CreateInstanceID();
    ownerObject->_instanceID = instanceID;
}

YAML::Node EGameObjectFactory::MakeYamlToGameObject(GameObject* gameObject)
{
    if (typeid(*gameObject) == typeid(GameObject))
    {
        YAML::Node objectNode;
        objectNode["SerializeVersion"] = 1;
        objectNode["Type"] = typeid(GameObject).name();
        objectNode["Prefab"] = gameObject->_prefabGuid.string();
        objectNode["ReflectFields"] = gameObject->SerializedReflectFields();
        {
            YAML::Node transformNode;
            transformNode["ReflectFields"] = gameObject->_transform.SerializedReflectFields();
            objectNode["Transform"] = transformNode;
        }
        return objectNode;
    }
    else
    {
#ifdef _DEBUG
        assert(!"유효하지 않는 오브젝트 타입입니다.");
#else   
        __debugbreak();
#endif // _DEBUG
        return YAML::Node();
    }  
}

std::shared_ptr<GameObject> EGameObjectFactory::MakeGameObjectToYaml(YAML::Node* pObjectNode)
{
    YAML::Node& objectNode = *pObjectNode;

    const int SerializeVersion = objectNode["SerializeVersion"].as<int>();
    std::string Type           = objectNode["Type"].as<std::string>();

    std::shared_ptr<GameObject> object = MakeGameObject(Type);
    ResetGameObject(object.get(), "null");
    ParsingYamlToGameObject(object.get(), objectNode);
    return object;
}

void EGameObjectFactory::ParsingYamlToGameObject(GameObject* pObject, const YAML::Node& objectNode) 
{
    const int SerializeVersion = objectNode["SerializeVersion"].as<int>();

    if (objectNode["Prefab"])
    {
        pObject->_prefabGuid = objectNode["Prefab"].as<std::string>();
    }
    std::string ReflectFields = objectNode["ReflectFields"].as<std::string>(); 
    yyjson_doc* jsonDoc  = yyjson_read(ReflectFields.c_str(), ReflectFields.size(), 0);
    yyjson_val* jsonRoot = yyjson_doc_get_root(jsonDoc);
    yyjson_val* jsonName = yyjson_obj_get(jsonRoot , "_name");
    const char* nameStr = yyjson_get_str(jsonName);
    ESceneManager::Engine::RenameGameObject(pObject, nameStr);
    yyjson_doc_free(jsonDoc); 

    if (SerializeVersion == 0)
    {
        //_tags 맴버 추가 이전
        struct ReflectFieldsVer0
        {
            std::string _name       = STR_NULL;
            bool        _activeSelf = true;
            bool        _isStatic   = false;
            //std::set<std::string> _tags; //"Version = 1"부터 추가됨
        };

        //구 버전 구조체로 역직렬화
        ReflectFieldsVer0 data;
        ReflectHelper::json::DeserializedObjet(data, ReflectFields);

        //두개 데이터 통합
        auto& objectReflectFields = *pObject->ReflectFields.Get();
        objectReflectFields = rfl::as<GameObject::reflect_fields_struct>(data, objectReflectFields);
    }
    else
    {
        bool result = pObject->DeserializedReflectFields(ReflectFields);
        if (false == result)
        {
            __debugbreak(); //역직렬화 실패.
        }
    }

    {
        YAML::Node  transformNode = objectNode["Transform"].as<YAML::Node>();
        std::string ReflectFields = transformNode["ReflectFields"].as<std::string>();
        pObject->_transform.DeserializedReflectFields(ReflectFields);
    }
}

int EGameObjectFactory::InstanceIDManager::CreateInstanceID()
{
    int instanceID = -1;
    instanceIdMutex.lock();
    if (_emptyID.empty())
    {
        instanceID = _backID++;
    }
    else
    {
        std::sort(_emptyID.begin(), _emptyID.end(), [](int a, int b) { return a > b; });
        instanceID = _emptyID.back();
        _emptyID.pop_back();
    } 
    instanceIdMutex.unlock();
    return instanceID;
}

void EGameObjectFactory::InstanceIDManager::ReturnInstanceID(int id)
{
    instanceIdMutex.lock();
    _emptyID.push_back(id);
    instanceIdMutex.unlock();
}

const std::vector<std::string>& EGameObjectFactory::Engine::GetGameObjectKeys()
{
    return engineCore->GameObjectFactory._NewGameObjectKeyVec;
}


