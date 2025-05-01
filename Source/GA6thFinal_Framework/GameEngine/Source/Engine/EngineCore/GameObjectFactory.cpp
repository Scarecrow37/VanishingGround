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
    UmFileSystem.RegisterFileEventNotifier(&UmGameObjectFactory, {EGameObjectFactory::PREFAB_EXTENSION});
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
}

void EGameObjectFactory::OnFileRegistered(const File::Path& path)
{
    File::Guid guid = path.ToGuid();
    YAML::Node yamlData = YAML::LoadFile(path.string());
    _prefabObjectMap[guid] = MakeObjectsGraphToYaml(&yamlData, true);
    WritePrefabGuid(path, yamlData);
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
    auto& guidQueue = _prefabGuidQueue[path];
    WritePrefabGuid(path, yamlData);
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

YAML::Node EGameObjectFactory::SerializeToYaml(GameObject* gameObject)
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
    Transform::ForeachBFS(
        gameObject->_transform, 
        [&](Transform* curr) 
        {
            //오브젝트 직렬화
            YAML::Node objectNode = MakeYamlToGameObject(&curr->gameObject);
            //컴포넌트들 직렬화
            for (auto& component : curr->gameObject->_components)
            {
                YAML::Node componentNode = UmComponentFactory.SerializeToYaml(component.get());
                objectNode["Components"].push_back(componentNode);
            }
            //Transform 직렬화
            transformParentLevelMap[curr] = parentIndex;
            YAML::Node transformNode = objectNode["Transform"].as<YAML::Node>();
            transformNode["TransformIndex"] = parentIndex;
            if (curr->Parent != nullptr)
            {
                transformNode["ParentIndex"] = transformParentLevelMap[curr->Parent];
            }                     
            ++parentIndex;
            nodes.push_back(objectNode);
        });
    return nodes;
}

std::vector<std::shared_ptr<GameObject>> EGameObjectFactory::MakeObjectsGraphToYaml(YAML::Node* pObjectNode, bool useResource) 
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
    std::map<int, Transform*>   transformParentLevelMap;
    std::shared_ptr<GameObject> currObject;
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
        ParsingYaml(currObject.get(), currNode);

        // 프리팹 추적
        if (currNode["Prefab"])
        {
            File::Guid prefab = currNode["Prefab"].as<std::string>();
            if (prefab != STR_NULL)
            {
                std::vector<std::weak_ptr<GameObject>>& instanceList = _prefabInstanceList[prefab];
                instanceList.emplace_back(currObject);
                currObject->_prefabGuid = prefab;
            }
        }

        // 컴포넌트들 역직렬화
        if (currNode["Components"])
        {
            YAML::Node componentNodes = currNode["Components"].as<YAML::Node>();
            for (auto componentNode : componentNodes)
            {
                YAML::Node& currComponentNode = componentNode;
                if (useResource == false)
                {
                    UmComponentFactory.AddComponentToYamlLifeCycle(currObject.get(), &currComponentNode);
                }
                else
                {
                    UmComponentFactory.AddComponentToYamlNow(currObject.get(), &currComponentNode);
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
            currObject->_transform.SetParent(pParent);
        }
        makeList.push_back(currObject);
    }
    return makeList;
}

std::shared_ptr<GameObject> EGameObjectFactory::DeserializeToYaml(YAML::Node* pObjectNode)
{
    auto makeList = MakeObjectsGraphToYaml(pObjectNode);
    for (auto& ptr : makeList)
    {
        ESceneManager::Engine::AddGameObjectToLifeCycle(ptr);
    }
    return makeList[0];
}

std::shared_ptr<GameObject> EGameObjectFactory::DeserializeToGuid(const File::Guid& guid)
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
        std::string message = std::format("{} {}", u8"존재하지 않는 프리팹입니다."_c_str, guid.ToPath().string());
        UmLogger.Log(LogLevel::LEVEL_WARNING, message);
        return nullptr;
    }
    YAML::Node yamlData = SerializeToYaml(iter->second[0].get());
    auto pObject = DeserializeToYaml(&yamlData);
    return pObject;
}

void EGameObjectFactory::WriteGameObjectFile(Transform* transform, std::string_view outPath)
{
    namespace fs     = std::filesystem;
    using fsPath     = std::filesystem::path;
    fsPath writePath = UmFileSystem.GetAssetPath();
    writePath /= outPath;
    writePath /= transform->gameObject->ToString();
    writePath.replace_extension(PREFAB_EXTENSION);
    if (fs::exists(writePath) == true)
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
    }
    _prefabGuidQueue[writePath].emplace_back(transform->gameObject->GetWeakPtr());
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
            _prefabInstanceList[guid].emplace_back(targetObject->GetWeakPtr());
            targetObject->_prefabGuid = guid;
            return true;
        }
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

bool EGameObjectFactory::IsOverrideField(void* pField)
{
    bool result = false;
    if (_prefabInstanceOverride.find(pField) != _prefabInstanceOverride.end())
    {
        result = true;
    }
    return result;
}

bool EGameObjectFactory::SetOverrideFlag(void* pField)
{
    auto [iter, result] = _prefabInstanceOverride.insert(pField);
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
        ownerObject->_ownerScene = STR_NULL;
    }   
    ownerObject->ReflectFields->_name = name;
    ownerObject->ReflectFields->_isStatic = false;
    ownerObject->ReflectFields->_activeSelf = true;
  
    //인스턴스 아이디 부여
    int instanceID = -1;
    if (instanceIDManager.EmptyID.empty())
    {
        instanceID = instanceIDManager.BackID++;
    }
    else
    {
        instanceID = instanceIDManager.EmptyID.back();
        instanceIDManager.EmptyID.pop_back();
    }
    ownerObject->_instanceID = instanceID;
}

YAML::Node EGameObjectFactory::MakeYamlToGameObject(GameObject* gameObject)
{
    if (typeid(*gameObject) == typeid(GameObject))
    {
        YAML::Node objectNode;
        objectNode["SerializeVersion"] = 0;
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
    ParsingYaml(object.get(), objectNode);
    return object;
}

void EGameObjectFactory::ParsingYaml(GameObject* pObject, YAML::Node& objectNode) 
{
    if (objectNode["Prefab"])
    {
        pObject->_prefabGuid = objectNode["Prefab"].as<std::string>();
    }
    std::string ReflectFields = objectNode["ReflectFields"].as<std::string>();
    pObject->DeserializedReflectFields(ReflectFields);
    {
        YAML::Node  transformNode = objectNode["Transform"].as<YAML::Node>();
        std::string ReflectFields = transformNode["ReflectFields"].as<std::string>();
        pObject->_transform.DeserializedReflectFields(ReflectFields);
    }
}

void EGameObjectFactory::Engine::ReturnInstanceID(int id)
{
    std::vector<int>& emptyID = engineCore->GameObjectFactory.instanceIDManager.EmptyID;
    emptyID.push_back(id);
    std::sort(emptyID.begin(), emptyID.end(), [](int a, int b)
        {
            return a > b; //내림차순 정렬
        });
}

const std::vector<std::string>& EGameObjectFactory::Engine::GetGameObjectKeys()
{
    return engineCore->GameObjectFactory._NewGameObjectKeyVec;
}

