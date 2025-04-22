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

void EGameObjectFactory::OnFileAdded(const File::Path& path) 
{
    _prefabDataMap[path.ToGuid()] = YAML::LoadFile(path.string());
    std::string message = std::format("Prefab Added : {}", path.string());
    UmEngineLogger.Log(LogLevel::LEVEL_TRACE, message.c_str());
}

void EGameObjectFactory::OnFileModified(const File::Path& path) 
{
    _prefabDataMap[path.ToGuid()] = YAML::LoadFile(path.string());
    std::string message = std::format("Prefab Modified : {}", path.string());
    UmEngineLogger.Log(LogLevel::LEVEL_TRACE, message.c_str());
}

void EGameObjectFactory::OnFileRemoved(const File::Path& path) 
{
    _prefabDataMap.erase(path.ToGuid());
    std::string message = std::format("Removed UmPrefab : {}", path.string());
    UmEngineLogger.Log(LogLevel::LEVEL_TRACE, message.c_str());
}

void EGameObjectFactory::OnFileRenamed(const File::Path& oldPath, const File::Path& newPath) 
{
    std::string message = std::format("Renamed UmPrefab {} to {}", oldPath.string(), newPath.string());
    UmEngineLogger.Log(LogLevel::LEVEL_TRACE, message.c_str());
}

void EGameObjectFactory::OnFileMoved(const File::Path& oldPath, const File::Path& newPath) 
{
    std::string message = std::format("Moved UmPrefab {} to {}", oldPath.string(), newPath.string());
    UmEngineLogger.Log(LogLevel::LEVEL_TRACE, message.c_str());
}

EGameObjectFactory::EGameObjectFactory()
{
    RegisterGameObjects();
}

EGameObjectFactory::~EGameObjectFactory() = default;

std::shared_ptr<GameObject> EGameObjectFactory::NewGameObject(std::string_view typeid_name, std::string_view name)
{
    auto sptr_object = MakeGameObject(typeid_name);
    ResetGameObject(sptr_object.get(), name);
    ESceneManager::Engine::AddGameObjectToLifeCycle(sptr_object);
    return sptr_object;
}

YAML::Node EGameObjectFactory::SerializeToYaml(GameObject* gameObject)
{
    if (UmComponentFactory.HasScript() == false)
    {
        UmEngineLogger.Log(LogLevel::LEVEL_ERROR,
                           u8"스크립트 빌드를 해주세요."_c_str);
        return YAML::Node();
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

std::shared_ptr<GameObject> EGameObjectFactory::DeserializeToYaml(YAML::Node* pGameObjectNode)
{
    if (UmComponentFactory.HasScript() == false)
    {
        UmEngineLogger.Log(LogLevel::LEVEL_ERROR,
                           u8"스크립트 빌드를 해주세요."_c_str);
        return nullptr;
    }

    YAML::Node& nodes = *pGameObjectNode;
    std::map<int, Transform*> transformParentLevelMap;
    std::shared_ptr<GameObject> currObject;
    for (auto node : nodes)
    {
        //오브젝트 생성
        YAML::Node& currNode = node;
        YAML::Node transformNode = currNode["Transform"].as<YAML::Node>();
        currObject = MakeGameObjectToYaml(&currNode);

        //컴포넌트들 역직렬화
        if (currNode["Components"])
        {
            YAML::Node componentNodes = currNode["Components"].as<YAML::Node>();
            for (auto componentNode : componentNodes)
            {
                YAML::Node& currComponentNode = componentNode;
                UmComponentFactory.DeserializeToYaml(currObject.get(),
                                                     &currComponentNode);
            }
        }

        //Transform 역직렬화
        int TransformIndex = transformNode["TransformIndex"].as<int>();
        transformParentLevelMap[TransformIndex] = &currObject->_transform;
        if (transformNode["ParentIndex"])
        {
            int        ParentIndex = transformNode["ParentIndex"].as<int>();
            Transform* pParent     = transformParentLevelMap[ParentIndex];
            currObject->_transform.SetParent(pParent);
        }
        ESceneManager::Engine::AddGameObjectToLifeCycle(currObject);
    }
    return transformParentLevelMap[0]->gameObject->GetWeakPtr().lock();
}

std::shared_ptr<GameObject> EGameObjectFactory::DeserializeToGuid(const File::Guid& guid)
{
    auto iter = _prefabDataMap.find(guid);
    if (iter == _prefabDataMap.end())
    {
        std::string message = std::format("{} {}", u8"존재하지 않는 프리팹입니다."_c_str, guid.ToPath().string());
        UmEngineLogger.Log(LogLevel::LEVEL_ERROR, message);
        return nullptr;
    }
    return DeserializeToYaml(&iter->second);
}

void EGameObjectFactory::WriteGameObjectFile(Transform* transform, std::string_view outPath)
{
    namespace fs     = std::filesystem;
    using fsPath     = std::filesystem::path;
    fsPath writePath = outPath;
    writePath /= transform->gameObject->ToString();
    writePath.replace_extension(PREFAB_EXTENSION);
    if (fs::exists(outPath) == true)
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
}

std::shared_ptr<GameObject> EGameObjectFactory::MakeGameObject(
    std::string_view typeid_name)
{
    std::shared_ptr<GameObject> newObject;
    auto findIter = _NewGameObjectFuncMap.find(typeid_name.data());
    if (findIter != _NewGameObjectFuncMap.end())
    {
        auto& [key, NewObjectFunc] = *findIter;
        newObject.reset(NewObjectFunc());
    }
    newObject->_weakPtr = newObject;
    return newObject;
}

void EGameObjectFactory::ResetGameObject(
    GameObject* ownerObject, 
    std::string_view name)
{
    ownerObject->_ownerScene = engineCore->SceneManager.GetMainScene().Name;
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
        objectNode["Type"]             = typeid(GameObject).name();
        objectNode["ReflectFields"] = gameObject->SerializedReflectFields();
        {
            YAML::Node transformNode;
            transformNode["ReflectFields"] = gameObject->_transform.SerializedReflectFields();
            objectNode["Transform"] = transformNode;
        }
        {
            
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

    std::string ReflectFields = objectNode["ReflectFields"].as<std::string>();
    object->DeserializedReflectFields(ReflectFields);
    {
        YAML::Node transformNode = objectNode["Transform"].as<YAML::Node>();
        std::string ReflectFields = transformNode["ReflectFields"].as<std::string>();
        object->_transform.DeserializedReflectFields(ReflectFields);
    }
    return object;
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

