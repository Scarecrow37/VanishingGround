#include "pch.h"
#include "Engine/GraphicsCore/Model.h"
#include "UmScripts.h"
using namespace Global;
using namespace u8_literals;

void Scene::IsDirty_property_setter(const std::remove_cvref_t<bool>& value) 
{
#ifdef _UMEDITOR
    if (false == editorModule->PlayMode.IsPlay())
    {
        _isDirty = value;
    }
#endif  
}

bool ESceneManager::RootGameObjectsFilter(GameObject* obj, std::string_view scenePath)
{
    return obj->_ownerScene == scenePath.data() && obj->transform->Parent == nullptr;
}

std::filesystem::path ESceneManager::GetSettingFilePath()
{
    std::filesystem::path path = UmFileSystem.GetSettingPath();
    path /= SETTING_FILE_NAME;
    return path;
}

ESceneManager::ESceneManager() 
{
   
}
ESceneManager::~ESceneManager()
{
    
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
#ifdef _UMEDITOR
    _isPlay = editorModule->PlayMode.IsPlay();
#endif
    SceneResourceManager::Update(ResourceManager);
    UmResourceManager.Update();
    ObjectsAddRuntime();
    ObjectsOnEnable();
    ObjectsAwake();
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

    if (UmSceneManager.InsertGameObjectMap(gameObject))
    {
        Global::engineCore->SceneManager._addGameObjectsQueue.push_back(gameObject);
    }
}

void ESceneManager::Engine::AddComponentToLifeCycle(std::shared_ptr<Component> component)
{
    Global::engineCore->SceneManager._addComponentsQueue.push_back(component);
}

void ESceneManager::Engine::SetGameObjectActive(GameObject* pObject, bool value)
{
    ESceneManager& sceneManager = UmSceneManager;
    if (nullptr != pObject)
    {
        GameObject* gameObject = pObject;
        if (gameObject->ReflectFields->_activeSelf != value)
        {
            //컴포넌트들의 On__able 함수를 호출하도록 합니다.
            auto& [WaitSet, WaitVec, WaitValue] = value ? sceneManager._onEnableQueue : sceneManager._onDisableQueue;
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
                                auto [iter, result] = WaitSet.insert(component.get());
                                if (result)
                                {
                                    WaitVec.emplace_back(component.get());
                                }
                            }
                        }

                        // 메시 컴포넌트들은 meshRenderer의 SetActive도 변경해야함.
                        if (Component::Type::RENDER == component->_type)
                        {
                            auto& meshActiveQueue = value ? sceneManager._meshSetActiveQueue.first
                                                          : sceneManager._meshSetActiveQueue.second;
                            MeshComponent* meshComponent  = static_cast<MeshComponent*>(component.get());
                            meshActiveQueue.push_back(meshComponent->Renderer.get());
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
    ESceneManager& sceneManager = UmSceneManager;
    if (component && component->ReflectFields->_enable != value)
    {
        //컴포넌트의 On__able 함수를 호출하도록 합니다.
        auto& [WaitSet, WaitVec, WaitValue] = value ? sceneManager._onEnableQueue : sceneManager._onDisableQueue;
        WaitValue.emplace_back(&component->ReflectFields->_enable);
        if (component->gameObject->ActiveInHierarchy == true)
        {
            auto [iter, result] = WaitSet.insert(component);
            if (result)
            {
                WaitVec.push_back(component);
            }
            
            // 메시 컴포넌트들은 meshRenderer의 SetActive도 변경해야함.
            if (Component::Type::RENDER == component->_type)
            {
                auto& meshActiveQueue =
                    value ? sceneManager._meshSetActiveQueue.first : sceneManager._meshSetActiveQueue.second;
                MeshComponent* meshComponent = static_cast<MeshComponent*>(component);
                meshActiveQueue.push_back(meshComponent->Renderer.get());
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
}

const std::vector<std::shared_ptr<GameObject>>& ESceneManager::Engine::GetRuntimeObjects()
{
   return engineCore->SceneManager._runtimeObjects;
}

void ESceneManager::Engine::DestroyObject(Component* component)
{
    UmSceneManager.AddDestroyComponentQueue(component);
}

void ESceneManager::Engine::DestroyObject(Component& component)
{
    DestroyObject(&component);
}

void ESceneManager::Engine::DestroyObject(GameObject* gameObject)
{
    if constexpr (Application::IsEditor())
    {
        UmCommandManager.Do<DestroyGameObjectCommand>(gameObject);
    }
    else
    {
        UmSceneManager.AddDestroyObjectQueue(gameObject);
    }
}

void ESceneManager::Engine::DestroyObject(GameObject& gameObject)
{
    DestroyObject(&gameObject);
}

void ESceneManager::Engine::DontDestroyOnLoadObject(GameObject* gameObject)
{
    ESceneManager& SceneManager = engineCore->SceneManager;
    if (true == SceneManager._isPlay)
    {
        auto find = SceneManager._scenesMap.find(DONT_DESTROY_ON_LOAD_SCENE_NAME);
        Scene* pDontDestroyScene = nullptr;
        if (find == SceneManager._scenesMap.end())
        {
            pDontDestroyScene = &SceneManager._scenesMap[DONT_DESTROY_ON_LOAD_SCENE_NAME];
            pDontDestroyScene->_isDontDestroyOnLoad = true;
        }
        else
        {
            pDontDestroyScene = &find->second;
        }
        if (pDontDestroyScene->isLoaded == false)
        {
            pDontDestroyScene->_isLoaded = true;
        }
        gameObject->_ownerScene = DONT_DESTROY_ON_LOAD_SCENE_NAME;
    }
}

void ESceneManager::Engine::DontDestroyOnLoadObject(GameObject& gameObject)
{
    DontDestroyOnLoadObject(&gameObject);
}

std::string& ESceneManager::Engine::GetStartSceneSetting()
{
    return UmSceneManager._setting.StartScene;
}

void ESceneManager::Engine::LoadStartScene() 
{
    ESceneManager& sceneManager = UmSceneManager;
    std::string& loadScene = Application::IsEditor() ? sceneManager._setting.MainScene : sceneManager._setting.StartScene;
    File::Path path = loadScene;
    File::Guid guid = path.ToGuid();
    auto findGuid = sceneManager._scenesMap.find(guid);
    if (loadScene != STR_NULL && findGuid != sceneManager._scenesMap.end())
    {
        if (UmComponentFactory.HasScript() == false)
        {
            if (UmComponentFactory.InitalizeComponentFactory() == false)
            {
                return;
            }
        }
        sceneManager.LoadScene(loadScene);
    }
}

void ESceneManager::Engine::SwapPrefabInstance(GameObject* original, GameObject* remake)
{
    ESceneManager& sceneManager = UmSceneManager;
    int index = original->GetInstanceID();
    std::shared_ptr<GameObject>& sOrigin = sceneManager._runtimeObjects[index];
    std::shared_ptr<GameObject>  sRemake = remake->GetWeakPtr().lock();
    std::swap(sOrigin->_instanceID, sRemake->_instanceID);
    std::swap(sOrigin->_ownerScene, sRemake->_ownerScene);
    std::swap(sOrigin, sRemake);
    std::string objectData = sRemake->SerializedReflectFields();
    sOrigin->DeserializedReflectFields(objectData);
    sOrigin->_transform = sRemake->_transform;
    sceneManager.EraseGameObjectMap(sRemake);
    sceneManager.InsertGameObjectMap(sOrigin);

    for (int i = 0; i < sOrigin->GetComponentCount(); ++i)
    {
        Component* component = sOrigin->GetComponentAtIndex<Component>(i);
        if (component)
        {
            component->_initFlags.SetAwake();
            component->_initFlags.SetStart();
        }
    }
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

        for (auto& obj : _runtimeObjects)
        {
            if (obj)
            {
                if (obj->_ownerScene == DONT_DESTROY_ON_LOAD_SCENE_NAME)
                    continue;

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
    scene->IsDirty   = false;
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

Scene* ESceneManager::GetDontDestroyOnLoadScene() 
{
    auto findIter = _scenesMap.find(DONT_DESTROY_ON_LOAD_SCENE_NAME);
    if (findIter != _scenesMap.end())
    {
        return &findIter->second;
    }
    return nullptr;
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
    if (_isPlay)
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
}

void ESceneManager::ObjectsLateUpdate()
{
    if (_isPlay)
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
    if (_isPlay)
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
}

void ESceneManager::ObjectsOnEnable()
{
    auto& [OnEnableSet, OnEnableVec, OnEnableValue] = _onEnableQueue;
    auto& MeshEnableQueue = _meshSetActiveQueue.first;
    for (auto& value : OnEnableValue)
    {
        *value = true;  
    }

    for (auto& mesh : MeshEnableQueue)
    {
        if (nullptr != mesh)
        {
            mesh->SetActive(true);
        }
    }
    
    if (_isPlay)
    {
        for (auto& component : OnEnableVec)
        {
            component->OnEnable();
        }
    }

    OnEnableSet.clear();
    OnEnableVec.clear();
    OnEnableValue.clear();
    MeshEnableQueue.clear();
}

void ESceneManager::ObjectsOnDisable()
{
    auto& [OnDisableSet, OnDisableVec, OnDisableValue] = _onDisableQueue;
    auto& MeshDisableQueue = _meshSetActiveQueue.second;

    for (auto& value : OnDisableValue)
    {
        *value = false;
    }

    for (auto& mesh : MeshDisableQueue)
    {
        if (nullptr != mesh)
        {
            mesh->SetActive(false);
        }
    }

    if (_isPlay)
    {
        for (auto& component : OnDisableVec)
        {
            component->OnDisable();
        }
    }

    OnDisableSet.clear();
    OnDisableVec.clear();
    OnDisableValue.clear();
    MeshDisableQueue.clear();
}

void ESceneManager::ObjectsDestroy()
{
    //컴포넌트 삭제
    auto& [destroyComponentSet, destroyComponentQueue] = _destroyComponentsQueue;
    for (auto& destroyComponent : destroyComponentQueue)
    {
        //OnDestroy 대상 호출
        if (_isPlay)
        {
            if (destroyComponent->_gameObect->ActiveInHierarchy_property_getter())
            {

                if (destroyComponent->Enable)
                {
                    destroyComponent->OnDestroy();
                }
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
                if (_isPlay)
                {
                    if (component->Enable)
                    {
                        component->OnDestroy();
                    }
                }
                NotInitDestroyComponentEraseToWaitVec(component.get());
            }
        }

        //오브젝트 삭제
        int instanceID = destroyObject->GetInstanceID();
        std::shared_ptr<GameObject>& pObject = _runtimeObjects[instanceID];
        EraseGameObjectMap(pObject);
        pObject.reset();
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
        if (_isPlay)
        {
            _waitAwakeVec.push_back(component);
            _waitStartVec.push_back(component);
        }
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

bool ESceneManager::InsertGameObjectMap(std::shared_ptr<GameObject>& pInsertObject) 
{
    auto [iter, result] = _runtimeObjectsUnorderedMap[pInsertObject->ReflectFields->_name].insert(pInsertObject);
    if (result == false)
    {
        assert(!"이미 추가한 게임 오브젝트 입니다.");
    }
    return result;
}

void ESceneManager::EraseGameObjectMap(std::shared_ptr<GameObject>& pEraseObject)
{
    auto findIter = _runtimeObjectsUnorderedMap.find(pEraseObject->ReflectFields->_name);
    if (findIter == _runtimeObjectsUnorderedMap.end())
    {
        assert(!"유효하지 않는 오브젝트 이름입니다.");
    }
    findIter->second.erase(pEraseObject);
}

void ESceneManager::AddDestroyComponentQueue(Component* component) 
{
    auto& [set, vec]    = engineCore->SceneManager._destroyComponentsQueue;
    auto [iter, result] = set.insert(component);
    if (result)
    {
        vec.push_back(component);
    }
}

void ESceneManager::SetObjectOwnerScene(GameObject* object, std::string_view sceneName) 
{
    object->_ownerScene = sceneName;
}

void ESceneManager::AddDestroyObjectQueue(GameObject* gameObject) 
{
    auto& [set, vec] = engineCore->SceneManager._destroyObjectsQueue;
    Transform::ForeachDFS(gameObject->_transform, 
    [&set, &vec](Transform* pTransform) 
    {
        auto [iter, result] = set.insert(&pTransform->gameObject);
        if (result)
        {
            vec.push_back(&pTransform->gameObject);
        }
    });
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
        YAML::Node objectNodes = object;
        YAML::Node rootObjectNode = *objectNodes.begin();
        std::shared_ptr<GameObject> newObject = UmGameObjectFactory.DeserializeToSceneObject(object);
        if (nullptr == newObject)
        {
            UmLogger.Log(LogLevel::LEVEL_FATAL, u8"메모리 할당 실패."_c_str);
            __debugbreak();
            UmApplication.Quit();
            return false;
        }

        Transform::ForeachDFS(
        newObject->_transform,
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

void ESceneManager::WriteSceneToFile(Scene& scene, std::string_view outPath, bool isOverride)
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

bool ESceneManager::WriteUmSceneFile(Scene& scene, std::string_view sceneName, std::string_view outPath, bool isOverride)
{
#ifdef _UMEDITOR
    if (true == editorModule->PlayMode.IsPlay())
    {
        return false;
    }
#endif 

    namespace fs     = std::filesystem;
    using fsPath     = std::filesystem::path;
    fsPath writePath = UmFileSystem.GetAssetPath();
    writePath /= outPath;
    writePath /= sceneName;
    writePath.replace_extension(SCENE_EXTENSION);
   
    bool isExists = fs::exists(writePath);
    if (true == isExists && false == isOverride)
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
            if (true == isExists)
            {
                scene.IsDirty = false;
                File::Guid guid = UmFileSystem.GetGuidFromPath(writePath);
                _sceneDataMap[guid] = node;
            }
        }
        ofs.close();
    }
    return true;
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
        std::filesystem::path writePath = path;
        writePath = std::filesystem::relative(writePath, UmFileSystem.GetAssetPath()).parent_path();
        WriteSceneToFile(scene, writePath.string(), true);
    }
    
    if (_loadFuncEvent)
    {
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
            _loadFuncEvent();
            _loadFuncEvent = nullptr;
        }
    }
}

void ESceneManager::OnFileUnregistered(const File::Path& path) 
{
    File::Guid  guid = path.ToGuid();
    Scene& scene = _scenesMap[guid];
    std::string sceneName = scene.Name;
    EraseSceneGUID(sceneName, guid);
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
    RenameScene(scene, oldName, newName);

    bool isLoaded = scene.isLoaded;
    if (true == isLoaded)
    {
        ResetOwnerScene(oldPath.string(), newPath.string());
        CheckMainSceneRename(scene, newPath);
    }
}

void ESceneManager::OnFileMoved(const File::Path& oldPath, const File::Path& newPath)
{
    File::Guid guid  = newPath.ToGuid();
    Scene&     scene = _scenesMap[guid];
    if (true == scene.isLoaded)
    {
        ResetOwnerScene(oldPath.string(), newPath.string());
        CheckMainSceneRename(scene, newPath);
    }
}

void ESceneManager::RenameScene(Scene& scene, std::string_view oldName, std::string_view newName) 
{
    _scenesFindMap[oldName.data()].erase(scene._guid);
    if (_scenesFindMap[oldName.data()].empty() == true)
    {
        _scenesFindMap.erase(oldName.data());
    }
    _scenesFindMap[newName.data()].insert(scene._guid);
}

void ESceneManager::ResetOwnerScene(std::string_view oldPath, std::string_view newPath) 
{
    auto rootObjects = GetRootGameObjectsByPath(oldPath.data());
    for (auto& object : rootObjects)
    {
        object->_ownerScene = newPath.data();
    }
}

void ESceneManager::CheckMainSceneRename(Scene& renameScene, const File::Path& newPath) 
{
    if (_lodedSceneList.front() == &renameScene)
    {
        _setting.MainScene = newPath.string();
    }
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

void ESceneManager::OnRequestedSave() 
{
    SaveSettingFile();
}

void ESceneManager::OnRequestedLoad() 
{
    LoadSettingFile();
}

void ESceneManager::OnPostRequestedLoad() 
{
    Engine::LoadStartScene();
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

void ESceneManager::SceneResourceManager::Update(SceneResourceManager& manager) 
{
    ModelResources& models = manager._models;
    {
        std::pair<std::weak_ptr<MeshComponent>, File::Guid> curr;
        while (false == models.ModelLoadQueue.empty())
        {
            if (true == models.ModelLoadQueue.try_pop(curr))
            {
                auto& [weakPtr, guid] = curr;
                if (false == weakPtr.expired())
                {
                    std::shared_ptr<MeshComponent> pMeshComponent = weakPtr.lock();
                    if (nullptr != pMeshComponent->Renderer)
                    {
                        MeshRenderer& meshRenderer = *pMeshComponent->Renderer;
                        File::Path path = guid.ToPath();
                        if (false == path.IsNull())
                        {
                            if (models.ModelResource.find(guid) == models.ModelResource.end())
                            {
                                models.ModelResource[guid] = UmResourceManager.LoadResource<Model>(path.string());
                            }
                            meshRenderer.RegisterRenderQueue("Editor");
                            meshRenderer.LoadModel(path.wstring()); 
                            models.ModelUseComponentList[guid].push_back(pMeshComponent);
                        }
                        else
                        {
                            UmLogger.Log(LogLevel::LEVEL_WARNING, std::format("{}{}", guid.string(), (const char*)u8"는 존재하지 않는 리소스입니다."));
                        }
                    }
                }
            }
        }
    }

}

void ESceneManager::SceneResourceManager::RequestModelResource(const MeshComponent* meshComponent, const File::Guid& guid)
{
    if (auto sharedPtr = meshComponent->GetWeakPtr().lock())
    {
        if (0 <= sharedPtr->_gameObect->_instanceID)
        {
            std::weak_ptr<MeshComponent> weakPtr = std::static_pointer_cast<MeshComponent>(sharedPtr);
            auto pair = std::make_pair(weakPtr, guid);
            _models.ModelLoadQueue.push(pair);
        }
    }
}

void ESceneManager::SceneResourceManager::ClearModelResource() 
{
    for (auto& [guid, resource] : _models.ModelResource)
    {
        auto componentListIter = _models.ModelUseComponentList.find(guid);
        if (componentListIter != _models.ModelUseComponentList.end())
        {
            auto& [guid, list] = *componentListIter;
            for (auto& weakPtr : list)
            {
                if (false == weakPtr.expired())
                {
                    auto pComponent = weakPtr.lock();
                    if (nullptr != pComponent->Renderer)
                    {
                        pComponent->Renderer->SetDestroy();
                    }
                }
            }
        }
    }
    _models.ModelUseComponentList.clear();
    _models.ModelResource.clear();
}

ESceneManager::SceneResourceManager::SceneResourceManager() 
{

}

ESceneManager::SceneResourceManager::~SceneResourceManager() 
{

}

ESceneManager::DestroyGameObjectCommand::DestroyGameObjectCommand(GameObject* object) 
    : 
    UmCommand("Destroy Object")
{
    Transform::ForeachBFS(object->transform, 
    [this](Transform* curr) 
    {
        _destroyObjects.push_back(curr->gameObject->GetWeakPtr().lock());
    });
    auto& rootObject = _destroyObjects.front();
    _active = rootObject->ActiveSelf;
    _ownerSceneName = rootObject->GetOwnerSceneName();
}

ESceneManager::DestroyGameObjectCommand::~DestroyGameObjectCommand() = default;

void ESceneManager::DestroyGameObjectCommand::Execute() 
{
    auto& rootObject = _destroyObjects.front();
    rootObject->GetScene().IsDirty = true;

    int instanceID = rootObject->_instanceID;
    rootObject->ActiveSelf = false;
    for (auto& object : _destroyObjects)
    {
        UmSceneManager.SetObjectOwnerScene(object.get(), STR_NULL);
    }
    UmSceneManager.AddDestroyObjectQueue(rootObject.get());

    if (EditorHierarchyTool::HierarchyFocusObjWeak.lock() == rootObject)
    {
        std::weak_ptr<GameObject> empty;
        EditorHierarchyTool::HierarchyFocusObjWeak = empty;
        EditorInspectorTool::SetFocusObject(empty);
        _isFocus = true;
    }
}

void ESceneManager::DestroyGameObjectCommand::Undo() 
{
    auto& rootObject = _destroyObjects.front();
    rootObject->ActiveSelf = _active;
    for (auto& object : _destroyObjects)
    {
        UmSceneManager.SetObjectOwnerScene(object.get(), _ownerSceneName);
        rootObject->GetScene().IsDirty = true;
        ESceneManager::Engine::AddGameObjectToLifeCycle(object);
    }
    
    if (_isFocus)
    {
        EditorHierarchyTool::HierarchyFocusObjWeak = rootObject;
        EditorInspectorTool::SetFocusObject(rootObject);
    }
}


ESceneManager::NewGameObjectCommand::NewGameObjectCommand(std::string_view type_id, std::string_view name)
    : 
    UmCommand("New GameObject"), _typeName(type_id), _newName(name)
{
}

void ESceneManager::NewGameObjectCommand::Execute()
{
    if (nullptr == _newObject)
    {
        _newObject = UmGameObjectFactory.NewGameObject(_typeName, _newName);
        _ownerScene = _newObject->GetOwnerSceneName();
    }
    else
    {
        _newObject->ActiveSelf = _active;
        UmSceneManager.SetObjectOwnerScene(_newObject.get(), _ownerScene);
        ESceneManager::Engine::AddGameObjectToLifeCycle(_newObject);
    }
    Scene& scene  = _newObject->GetScene();
    scene.IsDirty = true;
}

void ESceneManager::NewGameObjectCommand::Undo()
{
    int instanceID = _newObject->_instanceID;
    Scene& scene  = _newObject->GetScene();
    scene.IsDirty = true;
    _active = _newObject->ActiveSelf;
    _newObject->ActiveSelf = false;
    _newObject->transform->DetachChildren();
    UmSceneManager.SetObjectOwnerScene(_newObject.get(), STR_NULL);
    UmSceneManager.AddDestroyObjectQueue(_newObject.get());
    if (EditorHierarchyTool::HierarchyFocusObjWeak.lock() == _newObject)
    {
        std::weak_ptr<GameObject> empty;
        EditorHierarchyTool::HierarchyFocusObjWeak = empty;
        EditorInspectorTool::SetFocusObject(empty);
    }
}
