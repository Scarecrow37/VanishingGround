#include "pch.h"
using namespace Global;
using namespace u8_literals;
static constexpr const char* DONT_DESTROY_ON_LOAD_SCENE_NAME = "DontDestroyOnLoad";

bool ESceneManager::RootGameObjectsFilter(GameObject* obj, std::string_view scenePath)
{
    return obj->_ownerScene == scenePath.data() && obj->transform->Parent == nullptr;
}

std::filesystem::path ESceneManager::GetSettingFilePath()
{
    std::filesystem::path path = PROJECT_SETTING_PATH;
    path /= SETTING_FILE_NAME;
    return path;
}

ESceneManager::ESceneManager() 
{
    LoadSettingFile();
}
ESceneManager::~ESceneManager()
{
    if constexpr (Application::IsEditor())
    {
        SaveSettingFile();
    } 
}

void ESceneManager::LoadSettingFile() 
{
    std::string data;
    std::ifstream ifs(GetSettingFilePath());
    if (ifs.is_open() == true)
    {
        data = std::string(std::istreambuf_iterator<char>(ifs), {});
        auto result = rfl::json::read<decltype(_setting)>(data);
        if (result)
        {
            _setting = result.value();
        }
    }  
    ifs.close();
}

void ESceneManager::SaveSettingFile() const 
{
    std::filesystem::path settingPath = GetSettingFilePath();
    if (std::filesystem::exists(settingPath) == false)
    {
        std::filesystem::create_directories(settingPath.parent_path());
    }

    std::string data = rfl::json::write(_setting);
    std::ofstream ofs(settingPath, std::ios::trunc);
    if (ofs.is_open() == true)
    {
        ofs << data;
    }
    ofs.close();
}

void ESceneManager::Engine::RegisterFileEvents()
{
    //파일 관리자 등록
    UmFileSystem.RegisterFileEventNotifier(&UmSceneManager, {SCENE_EXTENSION});
}

void ESceneManager::Engine::CleanupSceneManager()
{
    engineCore->SceneManager._runtimeObjects.clear();
    engineCore->SceneManager._runtimeObjectsUnorderedMap.clear();
    engineCore->SceneManager._addGameObjectsQueue.clear();
    engineCore->SceneManager._addComponentsQueue.clear();
    engineCore->SceneManager._waitAwakeVec.clear();
    engineCore->SceneManager._waitStartVec.clear();
}

void ESceneManager::Engine::SceneUpdate()
{
    engineCore->SceneManager.SceneUpdate();
}

void ESceneManager::SceneUpdate()
{
    ObjectsAddRuntime();
    ObjectsOnEnable();
    ObjectsAwake();
    UmResourceManager.Update();
    ObjectsStart();
    while (ETimeSystem::Engine::TimeSystemFixedUpdate())
    {
        ObjectsFixedUpdate();
    }
    ObjectsUpdate();
    ObjectsLateUpdate();
    ObjectsApplicationQuit();
    ObjectsOnDisable();

    ObjectsDestroy();
    ObjectsMatrixUpdate();
}

void ESceneManager::Engine::AddGameObjectToLifeCycle(std::shared_ptr<GameObject> gameObject)
{
    if (gameObject->_ownerScene == STR_NULL)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"씬을 먼저 로드해주세요."_c_str);
        return;
    }

    auto [iter, result] = Global::engineCore->SceneManager._runtimeObjectsUnorderedMap[gameObject->ReflectFields->_name].insert(gameObject);
    if (result == false)
    {
        assert(!"이미 추가한 게임 오브젝트 입니다.");
    }
    else
    {
        Global::engineCore->SceneManager._addGameObjectsQueue.push_back(gameObject);
    }
}

void ESceneManager::Engine::AddComponentToLifeCycle(std::shared_ptr<Component> component)
{
    Global::engineCore->SceneManager._addComponentsQueue.push_back(component);
}

void ESceneManager::Engine::SetGameObjectActive(int instanceID, bool value)
{
    if (auto& gameObject = Global::engineCore->SceneManager._runtimeObjects[instanceID])
    {
        if (gameObject->ReflectFields->_activeSelf != value)
        {
            //컴포넌트들의 On__able 함수를 호출하도록 합니다.
            auto& [WaitSet, WaitVec, WaitValue] = value ? Global::engineCore->SceneManager._onEnableQueue : Global::engineCore->SceneManager._onDisableQueue;
            WaitValue.emplace_back(&gameObject->ReflectFields->_activeSelf);
                           
            if (value == true)
            {
                gameObject->ReflectFields->_activeSelf = true; //ActiveInHierarchy 검증용                        
            }

            Transform::ForeachDFS(gameObject->_transform, 
            [&](Transform* curr) 
            {
                if (curr->gameObject->ActiveInHierarchy == true)
                {
                    for (auto& component : curr->gameObject->_components)
                    {
                        if (component->_initFlags.IsAwake()   == true &&
                            component->ReflectFields->_enable == true)
                        {

                            {
                                auto [iter, result] =
                                    WaitSet.insert(component.get());
                                if (result)
                                {
                                    WaitVec.emplace_back(component.get());
                                }
                            }
                        }
                    }
                }                   
            });  

            if (value == true)
            {
                gameObject->ReflectFields->_activeSelf = false; //ActiveInHierarchy 검증용
            }
        }
    }
}

void ESceneManager::Engine::SetComponentEnable(Component* component, bool value)
{
    if (component && component->ReflectFields->_enable != value)
    {
        //컴포넌트의 On__able 함수를 호출하도록 합니다.
        auto& [WaitSet, WaitVec, WaitValue] = value ? engineCore->SceneManager._onEnableQueue : engineCore->SceneManager._onDisableQueue;
        WaitValue.emplace_back(&component->ReflectFields->_enable);
        if (component->gameObject->ActiveInHierarchy == true)
        {
            auto [iter, result] = WaitSet.insert(component);
            if (result)
            {
                WaitVec.push_back(component);
            }
        }
    }
}

std::weak_ptr<GameObject> ESceneManager::Engine::FindGameObjectWithName(std::string_view name)
{
    std::weak_ptr<GameObject> findObject;
    auto findIter = engineCore->SceneManager._runtimeObjectsUnorderedMap.find(name.data());
    if (findIter != engineCore->SceneManager._runtimeObjectsUnorderedMap.end() && !findIter->second.empty())
    {
        findObject = *findIter->second.begin();
    }
    return findObject;
}

std::vector<std::weak_ptr<GameObject>> ESceneManager::Engine::FindGameObjectsWithName(std::string_view name)
{
    std::vector<std::weak_ptr<GameObject>> findObjects;
    auto findIter = engineCore->SceneManager._runtimeObjectsUnorderedMap.find(name.data());
    if (findIter != engineCore->SceneManager._runtimeObjectsUnorderedMap.end() && !findIter->second.empty())
    {
        for (auto& obj : findIter->second)
        {
            findObjects.emplace_back(obj);
        }
    }
    return findObjects;
}

void ESceneManager::Engine::RenameGameObject(GameObject* gameObject, std::string_view newName)
{
    if (gameObject == nullptr)
        return;

    if (gameObject->ReflectFields->_name != newName)
    {
        auto& ObjectsNameMap = engineCore->SceneManager._runtimeObjectsUnorderedMap;
        auto mapIter = ObjectsNameMap.find(gameObject->ReflectFields->_name);
        if (mapIter != ObjectsNameMap.end())
        {
            const std::shared_ptr<GameObject>* sptr = nullptr;
            auto& [name, set] = *mapIter;
            for (auto& obj : set)
            {
                if (obj.get() == gameObject)
                {
                    sptr = &obj;
                    break;
                }
            }
            if (sptr)
            {
                ObjectsNameMap[newName.data()].insert(*sptr);
                set.erase(*sptr);
                gameObject->ReflectFields->_name = newName;
                if (set.empty())
                {
                    ObjectsNameMap.erase(mapIter);
                }
                return;
            }
        }
    }
    __debugbreak(); //이름 오류.
}

const std::vector<std::shared_ptr<GameObject>>& ESceneManager::Engine::GetRuntimeObjects()
{
   return engineCore->SceneManager._runtimeObjects;
}

void ESceneManager::Engine::DestroyObject(Component* component)
{
    auto& [set, vec] =  engineCore->SceneManager._destroyComponentsQueue;
    auto [iter, result] = set.insert(component);
    if (result)
    {
        vec.push_back(component);
    }
}

void ESceneManager::Engine::DestroyObject(Component& component)
{
    DestroyObject(&component);
}

void ESceneManager::Engine::DestroyObject(GameObject* gameObject)
{
    auto& [set, vec] = engineCore->SceneManager._destroyObjectsQueue;

   Transform::ForeachDFS(
        gameObject->_transform, 
        [&set, &vec](Transform* pTransform) 
        {
            auto [iter, result] = set.insert(&pTransform->gameObject);
            if (result)
            {
                vec.push_back(&pTransform->gameObject);
            }
        });
}

void ESceneManager::Engine::DestroyObject(GameObject& gameObject)
{
    DestroyObject(&gameObject);
}

void ESceneManager::Engine::DontDestroyOnLoadObject(GameObject* gameObject)
{
    ESceneManager& SceneManager = engineCore->SceneManager;

    auto find = SceneManager._scenesMap.find(DONT_DESTROY_ON_LOAD_SCENE_NAME);
    Scene* pDontDestroyScene = nullptr;
    if (find == SceneManager._scenesMap.end())
    {
        pDontDestroyScene = &SceneManager._scenesMap[DONT_DESTROY_ON_LOAD_SCENE_NAME];
    }
    else
    {
        pDontDestroyScene = &find->second;
    }
    pDontDestroyScene->_isLoaded = true;
    gameObject->_ownerScene      = DONT_DESTROY_ON_LOAD_SCENE_NAME;
}

void ESceneManager::Engine::DontDestroyOnLoadObject(GameObject& gameObject)
{
    DontDestroyOnLoadObject(&gameObject);
}

std::string& ESceneManager::Engine::GetStartSceneSetting()
{
    return UmSceneManager._setting.StartScene;
}

void ESceneManager::CreateEmptySceneAndLoad(std::string_view name, std::string_view outPath, const std::function<void()>& loadEvent) 
{
    if (UmComponentFactory.HasScript() == false)
    {
        if (UmComponentFactory.InitalizeComponentFactory() == false)
        {
            return;
        }
    }
    File::Path writePath = UmFileSystem.GetRootPath();
    writePath /= outPath;
    writePath /= name;
    writePath.replace_extension(SCENE_EXTENSION);

    if (std::filesystem::exists(writePath))
    {
        LoadScene(writePath.string());
        if (loadEvent)
        {
            loadEvent();
        }
    }
    else
    {
        WriteEmptySceneToFile(name, outPath);
        _setting.MainScene = writePath.generic_string();
        _loadFuncEvent     = loadEvent;
    }
}

void ESceneManager::LoadScene(std::string_view sceneName, LoadSceneMode mode)
{
    Scene* scene = GetSceneByName(sceneName);
    if (scene == nullptr)
    {
        return;
    }

    if (mode == LoadSceneMode::SINGLE)
    {
        _addComponentsQueue.clear();
        _addGameObjectsQueue.clear();
        _lodedSceneList.clear();

        for (auto& [guid, scene] : _scenesMap)
        {
            if (guid == DONT_DESTROY_ON_LOAD_SCENE_NAME)
                continue;

            scene._isLoaded = false;
            auto objects = scene.GetRootGameObjects();
            for (auto& obj : objects)
            {
                GameObject::Destroy(obj.get());
            }
        }
        _setting.MainScene = scene->Path;
    }
    else
    {
        Scene* mainScene = GetMainScene();
        if (mainScene == nullptr)
        {
            engineCore->Logger.Log(
                LogLevel::LEVEL_WARNING, 
                u8"메인 씬을 먼저 로드해주세요."_c_str);
            return;
        }
        if (scene->_isLoaded)
        {
            engineCore->Logger.Log(
                LogLevel::LEVEL_WARNING,
                u8"이미 로드된 씬은 추가 로드가 불가능합니다."_c_str);
            return;
        }
    }

    if (DeserializeToGuid(scene->_guid) == false)
    {
        return;
    }
    scene->_isLoaded = true;
    _lodedSceneList.push_back(scene);
}

void ESceneManager::UnloadScene(std::string_view sceneName) 
{
    Scene* scene = GetSceneByName(sceneName);
    if (scene == nullptr)
    {
        std::string message = std::format("{}{}", sceneName, u8"은 존재하지 않는 씬 입니다."_c_str);
        UmLogger.Log(LogLevel::LEVEL_WARNING, message);
        return;
    }

    if (scene->_isLoaded == false)
    {
        std::string message = std::format("{}{}", sceneName, u8"은 로드되지 않은 씬 입니다."_c_str);
        UmLogger.Log(LogLevel::LEVEL_WARNING, message);
        return;
    }

    if (Scene* mainScene = GetMainScene())
    {
        if (*scene == *mainScene)
        {
            std::string message = std::format("{}{}", sceneName, u8"은 메인 씬 이므로 언로드 할 수 없습니다."_c_str);
            UmLogger.Log(LogLevel::LEVEL_WARNING, message);
            return;
        }
    }

    scene->_isLoaded = false;
    auto objects = scene->GetRootGameObjects();
    for (auto& obj : objects)
    {
        GameObject::Destroy(obj.get());
    }
    std::erase(_lodedSceneList, scene);
}

Scene* ESceneManager::GetSceneByName(std::string_view name)
{
    auto find = _scenesFindMap.find(name.data());
    if (find != _scenesFindMap.end())
    {
        File::Guid guid = *find->second.begin();
        return &_scenesMap[guid];
    }
    else 
    {
        File::Path path = name.data();
        File::Guid guid = path.ToGuid();
        auto findGuid = _scenesMap.find(guid);
        if (findGuid != _scenesMap.end())
        {
            return &findGuid->second;
        }
    }
    std::string message = std::format("{}{}", name, u8"은 존재하지 않는 씬입니다."_c_str);
    UmLogger.Log(LogLevel::LEVEL_WARNING, message);
    return nullptr;
}

void ESceneManager::ObjectsAwake()
{
    for (auto& component : _waitAwakeVec)
    {
        if (component->_gameObect->ActiveInHierarchy_property_getter())
        {
            component->Awake();
            component->_initFlags.SetAwake();
            if (component->ReflectFields->_enable)
            {
                component->OnEnable();
            }
        }
    }
    std::erase_if(_waitAwakeVec, [](auto& component)
        {
            return component->_gameObect->ActiveInHierarchy_property_getter();
        });
}

void ESceneManager::ObjectsStart()
{
    for (auto& component : _waitStartVec)
    {
        if (component->_gameObect->ActiveInHierarchy_property_getter())
        {
            if (component->ReflectFields->_enable)
            {
                component->Start();
                component->_initFlags.SetStart();
            }
        }
    }
    std::erase_if(_waitStartVec, [](auto& component)
        {
            return component->_gameObect->ActiveInHierarchy_property_getter();
        });
}

void ESceneManager::ObjectsFixedUpdate()
{
    for (auto& obj : _runtimeObjects)
    {
        if (IsRuntimeActive(obj))
        {
            for (auto& component : obj->_components)
            {        
                if(component->Enable)
                    component->FixedUpdate();
            }
        }
    }
}

void ESceneManager::ObjectsUpdate()
{
    for (auto& obj : _runtimeObjects)
    {
        if (IsRuntimeActive(obj))
        {
            for (auto& component : obj->_components)
            {
                if (component->Enable)
                    component->Update();
            }
        }
    }
}

void ESceneManager::ObjectsLateUpdate()
{
    for (auto& obj : _runtimeObjects)
    {
        if (IsRuntimeActive(obj))
        {
            for (auto& component : obj->_components)
            {
                if (component->Enable)
                    component->LateUpdate();
            }
        }
    }
}

void ESceneManager::ObjectsMatrixUpdate()
{
    static std::unordered_set<Transform*> updateCheckSet;
    for (auto& obj : _runtimeObjects)
    {
        if (IsRuntimeActive(obj) && obj->_transform._hasChanged == true)
        {
            updateCheckSet.clear();
            Transform* root = obj->_transform._root ? obj->_transform._root : &obj->_transform;
            auto [iter, result] = updateCheckSet.insert(root);
            if (result == true)
            {
                root->UpdateMatrix();
            }
        }
    }
}

void ESceneManager::ObjectsApplicationQuit()
{
    if (Global::engineCore->App.IsQuit())
    {
        for (auto& obj : _runtimeObjects)
        {
            if (IsRuntimeActive(obj))
            {
                for (auto& component : obj->_components)
                {
                    if (component->Enable)
                        component->OnApplicationQuit();
                }
            }
        }
    }
}

void ESceneManager::ObjectsOnEnable()
{
    auto& [OnEnableSet, OnEnableVec, OnEnableValue] = _onEnableQueue;
    for (auto& value : OnEnableValue)
    {
        *value = true;  
    }
    for (auto& component : OnEnableVec)
    {
        component->OnEnable();
    }
    OnEnableSet.clear();
    OnEnableVec.clear();
    OnEnableValue.clear();
}

void ESceneManager::ObjectsOnDisable()
{
    auto& [OnDisableSet, OnDisableVec, OnDisableValue] = _onDisableQueue;
    for (auto& value : OnDisableValue)
    {
        *value = false;
    }
    for (auto& component : OnDisableVec)
    {
        component->OnDisable();
    }
    OnDisableSet.clear();
    OnDisableVec.clear();
    OnDisableValue.clear();
}

void ESceneManager::ObjectsDestroy()
{
    //컴포넌트 삭제
    auto& [destroyComponentSet, destroyComponentQueue] = _destroyComponentsQueue;
    for (auto& destroyComponent : destroyComponentQueue)
    {
        //OnDestroy 대상 호출
        if (destroyComponent->_gameObect->ActiveInHierarchy_property_getter())
        {
            if (destroyComponent->Enable)
            {
                destroyComponent->OnDestroy();
            }
        }

        //해당 컴포넌트를 오브젝트 배열에서 삭제.
        std::vector<std::shared_ptr<Component>>& components = destroyComponent->_gameObect->_components;
        std::erase_if(
            components, 
            [destroyComponent](std::shared_ptr<Component>& component)
            {     
                return destroyComponent == component.get();
            }
        );

        NotInitDestroyComponentEraseToWaitVec(destroyComponent);
    }
    destroyComponentSet.clear();
    destroyComponentQueue.clear();

    //오브젝트 삭제
    auto& [destroyObjectSet, destroyObjectQueue] = _destroyObjectsQueue;
    for (auto& destroyObject : destroyObjectQueue)
    {
        //OnDestroy 대상 호출
        if (destroyObject->ActiveInHierarchy_property_getter())
        {
            for (auto& component : destroyObject->_components)
            {
                if (component->Enable)
                {
                    component->OnDestroy();
                }

                NotInitDestroyComponentEraseToWaitVec(component.get());
            }
        }

        //오브젝트 삭제
        int instanceID = destroyObject->GetInstanceID();
        std::shared_ptr<GameObject>& pObject = _runtimeObjects[instanceID];
        auto findIter = _runtimeObjectsUnorderedMap.find(destroyObject->ReflectFields->_name);
        if (findIter == _runtimeObjectsUnorderedMap.end())
        {
            assert(!"유효하지 않는 오브젝트 이름입니다.");
        }
        findIter->second.erase(pObject);
        pObject.reset();
        EGameObjectFactory::Engine::ReturnInstanceID(instanceID);
    }
    destroyObjectSet.clear();
    destroyObjectQueue.clear();

    //배열 정리
    while (_runtimeObjects.empty() == false && _runtimeObjects.back() == nullptr)
    {
        _runtimeObjects.pop_back();
    }
}

void ESceneManager::ObjectsAddRuntime()
{
    for (auto& gameObject : _addGameObjectsQueue)
    {
        int id = gameObject->_instanceID;
        if (id < 0)
        {
            __debugbreak(); //인스턴스 아이디 오류
        }
        if (id < _runtimeObjects.size() && _runtimeObjects[id])
        {
            __debugbreak(); //이미 존재하는 인스턴스 아이디.
        }

        if (_runtimeObjects.size() <= id)
        {
            _runtimeObjects.resize(id + 1);
        }
        _runtimeObjects[id] = gameObject;
    }
    _addGameObjectsQueue.clear();

    for (auto& component : _addComponentsQueue)
    {
        component->_gameObect->_components.emplace_back(component);
        _waitAwakeVec.push_back(component);
        _waitStartVec.push_back(component);
    }
    _addComponentsQueue.clear();
}

bool ESceneManager::IsRuntimeActive(std::shared_ptr<GameObject>& obj)
{
    return obj.get() != nullptr && obj->ActiveInHierarchy_property_getter();
}

void ESceneManager::NotInitDestroyComponentEraseToWaitVec(Component* destroyComponent)
{
    if (destroyComponent->_initFlags.IsAwake() == false)
    {
        std::erase_if(
            _waitAwakeVec,
            [destroyComponent](std::shared_ptr<Component>& component)
            {
                return component.get() == destroyComponent;
            }
        );
    }

    if (destroyComponent->_initFlags.IsStart() == false)
    {
        std::erase_if(
            _waitStartVec,
            [destroyComponent](std::shared_ptr<Component>& component)
            {
                return component.get() == destroyComponent;
            }
        );
    }

}

YAML::Node ESceneManager::SerializeToYaml(const Scene& scene)
{
    if (UmComponentFactory.HasScript() == false)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"스크립트를 빌드해주세요. :("_c_str);
        return YAML::Node();
    }

    Scene& targetScene = const_cast<Scene&>(scene);
    YAML::Node sceneNode;
    sceneNode["SerializeVersion"] = 0;
    sceneNode["Guid"] = scene._guid.string();

    auto rootObjects = scene.GetRootGameObjects();
    for (auto& object : rootObjects)
    {
        YAML::Node objectNode = UmGameObjectFactory.SerializeToYaml(object.get());
        sceneNode["GameObjects"].push_back(objectNode);
    }
    return sceneNode;
}

bool ESceneManager::DeserializeToYaml(YAML::Node* _sceneNode)
{
    if (UmComponentFactory.HasScript() == false)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"스크립트를 빌드해주세요. :("_c_str);
        return false;
    }

    YAML::Node& sceneNode = *_sceneNode;
    int SerializeVersion = sceneNode["SerializeVersion"].as<int>();
    File::Guid Guid = sceneNode["Guid"].as<std::string>();
    
    YAML::Node rootObjects = sceneNode["GameObjects"].as<YAML::Node>();
    for (auto object : rootObjects)
    {
        YAML::Node objectNode = object;
        auto newObject = UmGameObjectFactory.DeserializeToYaml(&objectNode);
        Transform::ForeachDFS(newObject->_transform,
        [&Guid](Transform* curr) 
        {
            curr->_gameObject._ownerScene = Guid.ToPath().string();
        });
    }
    return true;
}

bool ESceneManager::DeserializeToGuid(const File::Guid& guid)
{
    auto findIter = _sceneDataMap.find(guid);
    if (findIter == _sceneDataMap.end())
    {
        std::string messgae = std::format("{} : {}", u8"존재하지 않는 파일입니다."_c_str, guid.ToPath().string());
        UmLogger.Log(LogLevel::LEVEL_WARNING, messgae);
        return false;
    }
    return DeserializeToYaml(&findIter->second);
}

void ESceneManager::WriteSceneToFile(const Scene& scene, std::string_view outPath, bool isOverride)
{
    namespace fs = std::filesystem;
    std::string sceneName = scene.Name;
    bool result = WriteUmSceneFile(scene, sceneName, outPath, isOverride);
}

void ESceneManager::WriteEmptySceneToFile(std::string_view name, std::string_view outPath, bool isOverride)
{
    namespace fs = std::filesystem;
    Scene scene;
    bool result = WriteUmSceneFile(scene, name, outPath, isOverride);
}

bool ESceneManager::WriteUmSceneFile(const Scene& scene, std::string_view sceneName, std::string_view outPath, bool isOverride)
{
    namespace fs     = std::filesystem;
    using fsPath     = std::filesystem::path;
    fsPath writePath = UmFileSystem.GetRootPath();
    writePath /= outPath;
    writePath /= sceneName;
    writePath.replace_extension(SCENE_EXTENSION);
   
    if (fs::exists(writePath) == true && isOverride == false)
    {
        int result = MessageBox(UmApplication.GetHwnd(), L"파일이 이미 존재합니다. 덮어쓰겠습니까?",
                                L"파일이 존재합니다.", MB_YESNO);
        if (result != IDYES)
        {
            return false;
        }
    }
    fs::create_directories(writePath.parent_path());
    YAML::Node node = SerializeToYaml(scene);
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

void ESceneManager::OnFileRegistered(const File::Path& path) 
{
    File::Guid guid     = path.ToGuid();
    _sceneDataMap[guid] = YAML::LoadFile(path.string());
    YAML::Node& node    = _sceneDataMap[guid];

    Scene& scene = _scenesMap[path.ToGuid()];
    scene._guid  = guid;
    _scenesFindMap[scene.Name].insert(guid);
    std::string nodeGuid = node["Guid"].as<std::string>();
    if (nodeGuid != guid)
    {
        if (UmComponentFactory.HasScript() == false)
        {
            if (UmComponentFactory.InitalizeComponentFactory() == false)
            {
                return;
            }
        }    
        std::filesystem::path relativeRootPath = std::filesystem::relative(path, UmFileSystem.GetRootPath());
        WriteSceneToFile(scene, relativeRootPath.parent_path().string(), true);
    }
    
    std::string& loadScene = Application::IsEditor() ? _setting.MainScene : _setting.StartScene; 
    if (scene.isLoaded == false && path.string() == loadScene)
    {
        if (UmComponentFactory.HasScript() == false)
        {
            if (UmComponentFactory.InitalizeComponentFactory() == false)
            {
                return;
            }
        }
        LoadScene(path.string());
        if (_loadFuncEvent)
        {
            _loadFuncEvent();
            _loadFuncEvent = nullptr;
        }
    }
}

void ESceneManager::OnFileUnregistered(const File::Path& path) 
{

}

void ESceneManager::OnFileModified(const File::Path& path)
{
    _sceneDataMap[path.ToGuid()] = YAML::LoadFile(path.string());
}

void ESceneManager::OnFileRemoved(const File::Path& path) 
{
    File::Guid guid = path.ToGuid();
    Scene& scene = _scenesMap[guid];
    std::string sceneName = scene.Name;
    EraseSceneGUID(sceneName, guid);
}

void ESceneManager::OnFileRenamed(const File::Path& oldPath, const File::Path& newPath) 
{
    File::Guid guid = newPath.ToGuid();
    Scene& scene = _scenesMap[guid];
    std::string oldName = oldPath.stem().string();
    std::string newName = scene.Name;
    _scenesFindMap[oldName].erase(guid);
    if (_scenesFindMap[oldName].empty() == true)
    {
        _scenesFindMap.erase(oldName);
    }
    _scenesFindMap[newName].insert(guid);

    bool isLoaded = scene.isLoaded;
    if (isLoaded)
    {
        auto rootObjects = GetRootGameObjectsByPath(oldPath.string());
        for (auto& object : rootObjects)
        {
            object->_ownerScene = newPath.string();
        }
    }
}

void ESceneManager::OnFileMoved(const File::Path& oldPath, const File::Path& newPath) 
{

}

void ESceneManager::OnRequestedOpen(const File::Path& path) 
{
    LoadScene(path.string());
}

void ESceneManager::OnRequestedCopy(const File::Path& path) 
{

}

void ESceneManager::OnRequestedPaste(const File::Path& path) 
{

}

void ESceneManager::EraseSceneGUID(std::string_view sceneName, const File::Guid guid) 
{
    Scene* pScene = &_scenesMap[guid];
    auto objects = pScene->GetRootGameObjects();
    for (auto& obj : objects)
    {
        GameObject::Destroy(obj.get());
    }
    std::erase(_lodedSceneList, pScene);
    _scenesFindMap[sceneName.data()].erase(guid);
    if (_scenesFindMap[sceneName.data()].empty() == true)
    {
        _scenesFindMap.erase(sceneName.data());
    }
    _scenesMap.erase(guid);
    _sceneDataMap.erase(guid);
}
