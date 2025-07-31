#include "pch.h"
#include "UmScripts.h"

using namespace Global;
using namespace u8_literals;

void Scene::IsDirty_property_setter(const std::remove_cvref_t<bool>& value) 
{
#ifdef _UMEDITOR
    if (false == ESceneManager::Engine::IsPlayMode(UmSceneManager))
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
    std::filesystem::path path = UmFileSystem.GetBuildSettingPath();
    path /= SETTING_FILE_NAME;
    return path;
}

ESceneManager::ESceneManager() 
    : _mainCamera(nullptr) 
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
    UmFileSystem.RegisterFileEventSubscriber(&UmSceneManager, {SCENE_EXTENSION});
}

void ESceneManager::Engine::CleanupSceneManager()
{
    //실제 활성화 오브젝트
    engineCore->SceneManager._runtimeObjects.clear();
    engineCore->SceneManager._runtimeObjectsUnorderedMap.clear();

    //태그 맵
    engineCore->SceneManager._runtimeObjectsTagMap.clear();

    //파괴 큐
    engineCore->SceneManager._destroyObjectsQueue.first.clear();
    engineCore->SceneManager._destroyObjectsQueue.second.clear();
    engineCore->SceneManager._destroyComponentsQueue.first.clear();
    engineCore->SceneManager._destroyComponentsQueue.second.clear();
    
    //추가 큐
    engineCore->SceneManager._addGameObjectsQueue.clear();
    engineCore->SceneManager._addComponentsQueue.clear();

    //초기화 큐
    engineCore->SceneManager._waitAwakeVec.clear();
    engineCore->SceneManager._waitStartVec.clear();

    // OnEnable 큐
    auto& [enableSet, enableVec, enableValue] = engineCore->SceneManager._onEnableQueue;
    enableSet.clear();
    enableVec.clear();
    enableValue.clear();

    // OnDisable 큐
    auto& [disableSet, disableVec, disableValue] = engineCore->SceneManager._onDisableQueue;
    disableSet.clear();
    disableVec.clear();
    disableValue.clear();

    //하이러키 Update 큐
    auto& [enableUpdateSet, enableUpdateQueue] = engineCore->SceneManager._updateEnableQueue;
    enableUpdateSet.clear();
    enableUpdateQueue.clear();
    auto& [disableUpdateSet, disableUpdateQueue] = engineCore->SceneManager._updateDisableQueue;
    disableUpdateSet.clear();
    disableUpdateQueue.clear();

    //Render component들
    engineCore->SceneManager._runtimeMeshComponents.clear();
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
    SceneResourceManager::Engine::Update(ResourceManager);
    ObjectsAddRuntime();
    ObjectsOnEnable();
    ObjectsAwake();
    ObjectsStart();
    ObjectsInputUpdate();
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
    ObjectsAddLoadScene();
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
    EComponentFactory::Engine::PushBackComponentToObject(component);
}

void ESceneManager::Engine::SetGameObjectActive(GameObject* pObject, bool value)
{
    ESceneManager& sceneManager = UmSceneManager;
    if (nullptr != pObject)
    {
        GameObject* gameObject = pObject;
        if (gameObject->ReflectFields->_activeSelf != value)
        {
            //bool 변경 대기열에 추가
            auto& [WaitSet, WaitVec, WaitValue] = value ? sceneManager._onEnableQueue : sceneManager._onDisableQueue;
            WaitValue.emplace_back(&gameObject->ReflectFields->_activeSelf);   

            //ActiveInHierarchy Update 대기열에 추가
            auto& [UpdateSet, UpdateQueue] = value ? sceneManager._updateEnableQueue : sceneManager._updateDisableQueue;
            auto [iter, result] = UpdateSet.insert(gameObject);
            if (true == result)
            {
                UpdateQueue.push_back(gameObject);
            }

            //컴포넌트들의 On__able 함수를 호출하도록 합니다.
            if (value == true)
            {
                gameObject->ReflectFields->_activeSelf = true; //ActiveInHierarchy 검증용  
                GameObject::Engine::UpdateActiveInHierarchy(gameObject);
            }
            Transform::ForeachDFS(gameObject->_transform, 
            [&](Transform* curr) 
            {
                if (curr->gameObject->ActiveInHierarchy == true)
                {
                    for (auto& component : curr->gameObject->_components)
                    {
                        bool isEnable = component->ReflectFields->_enable;
                        if (true == isEnable)
                        {
                            if (component->_initFlags.IsAwake() == true)
                            {
                                auto [iter, result] = WaitSet.insert(component.get());
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
                GameObject::Engine::UpdateActiveInHierarchy(gameObject);
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
        }
    }
}

std::weak_ptr<GameObject> ESceneManager::Engine::FindGameObjectWithName(std::string_view name)
{
    std::weak_ptr<GameObject> findObject;
    auto findIter = engineCore->SceneManager._runtimeObjectsUnorderedMap.find(name.data());
    if (findIter != engineCore->SceneManager._runtimeObjectsUnorderedMap.end() && !findIter->second.empty())
    {
        for (auto& object : findIter->second)
        {
            if (true == object->IsValid())
            {
                findObject = object->GetWeakPtr();
                break;
            }
        }
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
            if (true == obj->IsValid())
            {
                findObjects.emplace_back(obj);
            }
        }
    }
    return findObjects;
}

std::weak_ptr<GameObject> ESceneManager::Engine::FindGameObjectWithTag(std::string_view tag)
{
    std::weak_ptr<GameObject> findObject;
    auto findIter = engineCore->SceneManager._runtimeObjectsTagMap.find(tag.data());
    if (findIter != engineCore->SceneManager._runtimeObjectsTagMap.end() && !findIter->second.empty())
    {
        for (auto& object : findIter->second)
        {
            if (true == object->IsValid())
            {
                findObject = object->GetWeakPtr();
                break;
            }
        }
    }
    return findObject;
}

std::vector<std::weak_ptr<GameObject>> ESceneManager::Engine::FindGameObjectsWithTag(std::string_view tag)
{
    std::vector<std::weak_ptr<GameObject>> findObjects;
    auto findIter = engineCore->SceneManager._runtimeObjectsTagMap.find(tag.data());
    if (findIter != engineCore->SceneManager._runtimeObjectsTagMap.end() && !findIter->second.empty())
    {
        for (auto& obj : findIter->second)
        {
            if (true == obj->IsValid())
            {
                findObjects.emplace_back(obj->GetWeakPtr());
            }
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
    UmSceneManager.AddDestroyObjectQueue(gameObject);
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
    path = std::filesystem::absolute(path);
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
        sceneManager.LoadScene(path.string());
    }
}

std::shared_ptr<GameObject> ESceneManager::Engine::SwapPrefabInstance(GameObject* original, GameObject* remake)
{
    ESceneManager& sceneManager = UmSceneManager;
    std::shared_ptr<GameObject> originObject; 
    if (original->IsValid())
    {
        int index = original->GetInstanceID();
        if (0 <= index && index < sceneManager._runtimeObjects.size())
        {
            std::shared_ptr<GameObject>& sOrigin = sceneManager._runtimeObjects[index];
            if (nullptr != sOrigin)
            {
                originObject = sOrigin;
                std::shared_ptr<GameObject> sRemake = remake->GetWeakPtr().lock();

                //오브젝트 정보 복사
                std::swap(sOrigin->_instanceID, sRemake->_instanceID);
                std::swap(sOrigin->_ownerScene, sRemake->_ownerScene);
                std::string objectData = sOrigin->SerializedReflectFields();
                sRemake->DeserializedReflectFields(objectData);

                //트렌스폼 정보 복사
                sRemake->_transform.CopyTransform(sOrigin->_transform, false);
                GameObject::Engine::UpdateActiveInHierarchy(sRemake.get());

                //컴포넌트 오버라이드
                for (int i = 0; i < sRemake->GetComponentCount(); ++i)
                {
                    Component* remakeComponent = sRemake->GetComponentAtIndex<Component>(i);
                    if (remakeComponent)
                    {
                        remakeComponent->_initFlags.SetAwake();
                        remakeComponent->_initFlags.SetStart();
                        Component* originComponent = sOrigin->GetComponentAtIndex<Component>(i);
                        if (originComponent)
                        {
                            std::string componentData = originComponent->SerializedReflectFields();
                            remakeComponent->DeserializedReflectFields(componentData);
                            remakeComponent->Reset();
                        }
                    }
                }

                std::swap(sOrigin, sRemake);
                sceneManager.EraseGameObjectMap(sRemake);
                sceneManager.InsertGameObjectMap(sOrigin);
            }
        }    
    }
    return originObject;
}

void ESceneManager::Engine::SetSceneSkyBoxGuid(Scene& scene, const File::Guid& skyBox)
{
    scene._skyBox = skyBox;
}

void ESceneManager::Engine::SetSceneSkyBoxPath(Scene& scene, std::string_view skyBoxPath) 
{
    File::Guid guid = UmFileSystem.GetGuidFromPath(skyBoxPath);
    if (false == guid.IsNull())
    {
        SetSceneSkyBoxGuid(scene, guid);
    }
}

void ESceneManager::Engine::UpdateMatrix(GameObject* gameObject) 
{
    gameObject->transform->UpdateMatrix();
}

bool ESceneManager::Engine::InsertGameObjectTag(GameObject* gameObject, std::string_view tag)
{
    auto [iter, result] = UmSceneManager._runtimeObjectsTagMap[tag.data()].insert(gameObject);
    return result;
}

bool ESceneManager::Engine::EraseGameObjectTag(GameObject* gameObject, std::string_view tag)
{
    auto tagIter = UmSceneManager._runtimeObjectsTagMap.find(tag.data());
    if (tagIter == UmSceneManager._runtimeObjectsTagMap.end())
    {
        return false;
    }

    std::unordered_set<GameObject*>& objectSet = tagIter->second;
    auto objIter = objectSet.find(gameObject);
    if (objIter == objectSet.end())
    {
        return false;
    }
   
    objectSet.erase(objIter);
    return true;
}

void ESceneManager::Engine::SetSceneMainCamera(CameraComponent* camera)
{
    ESceneManager& sceneManager = UmSceneManager;
    if (camera != sceneManager._mainCamera)
    {
        if (nullptr != sceneManager._mainCamera)
        {
            sceneManager._mainCamera->ResetMainCamera();
        }
        sceneManager._mainCamera = camera;
    }
}

void ESceneManager::Engine::ResetSceneMainCamera() 
{
    ESceneManager& sceneManager = UmSceneManager;
    if (sceneManager._mainCamera)
    {
        sceneManager._mainCamera = nullptr;
    }
}

CameraComponent* ESceneManager::Engine::GetMainCamera()
{
    return UmSceneManager._mainCamera;
}

ESceneManager::InputSystem& ESceneManager::Engine::GetInputSystem()
{
    return UmSceneManager._inputSystem;
}

void ESceneManager::Engine::PushRuntimeMeshComponent(MeshComponent* component) 
{
    if (component->_gameObject->IsValid())
    {
        const auto&                  componentWeak = component->GetWeakPtr().lock();
        std::weak_ptr<MeshComponent> weak          = std::static_pointer_cast<MeshComponent>(componentWeak);
        UmSceneManager._runtimeMeshComponents.push_back(weak);
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
    if (false == UmComponentFactory.HasScript())
    {
        UmComponentFactory.InitalizeComponentFactory();
    }

    Scene* scene = GetSceneByName(sceneName);
    if (scene == nullptr)
    {
        return;
    }

    if (mode == LoadSceneMode::SINGLE)
    {
        for (auto& obj : _runtimeObjects)
        {
            if (obj)
            {
                if (obj->_ownerScene == DONT_DESTROY_ON_LOAD_SCENE_NAME)
                    continue;

                GameObject::Destroy(obj.get());
            }
        }

        if (false == _lodedSceneList.empty())
        {
            _prevScene = _setting.MainScene;
            for (auto& scene : _lodedSceneList)
            {
                scene->_isLoaded = false;
            }
        }
        _setting.MainScene = scene->Path;
        _addComponentsQueue.clear();
        _addGameObjectsQueue.clear();
        _waitAwakeVec.clear();
        _waitStartVec.clear();
        _lodedSceneList.clear();
        UmCommandManager.Clear();
        SetRendererSkyBox(scene);
    }
    else
    {
        Scene* mainScene = GetMainScene();
        if (mainScene == nullptr)
        {
            engineCore->Logger.Log(LogLevel::LEVEL_WARNING, u8"메인 씬을 먼저 로드해주세요."_c_str);
            return;
        }
        if (scene->_isLoaded)
        {
            engineCore->Logger.Log(LogLevel::LEVEL_WARNING, u8"이미 로드된 씬은 추가 로드가 불가능합니다."_c_str);
            return;
        }
    }
    _nextSceneGuid = scene->_guid;
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
        if (component->_gameObject->ActiveInHierarchy_property_getter())
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
            return component->_gameObject->ActiveInHierarchy_property_getter();
        });
}

void ESceneManager::ObjectsStart()
{
    for (auto& component : _waitStartVec)
    {
        if (component->_gameObject->ActiveInHierarchy_property_getter())
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
            return component->_gameObject->ActiveInHierarchy_property_getter();
        });
}

void ESceneManager::ObjectsInputUpdate() 
{
    if (_isPlay)
    {
        _inputSystem.UpdateInput();
    }
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
    if (nullptr != _mainCamera)
    {
        if (true == _mainCamera->IsDirty())
        {
            _mainCamera->UpdatePerspective();
        }
        
        Transform* curr = &_mainCamera->transform;
        while (nullptr != curr)
        {
            if (true == curr->_hasChanged)
            {
                Transform* root = curr;
                if (nullptr != curr->_root)
                {
                    root = curr->_root;
                }
                updateCheckSet.insert(root);
                curr->UpdateMatrix();
                _mainCamera->UpdateView();
                break;
            }
            curr = curr->_parent;
        }
    }

    for (auto& obj : _runtimeObjects)
    {
        if (nullptr != obj && obj->_transform._hasChanged == true)
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

void ESceneManager::ObjectsAddLoadScene() 
{
    if (false == _nextSceneGuid.empty())
    {
        auto sceneIter = _scenesMap.find(_nextSceneGuid);
        if (sceneIter != _scenesMap.end())
        {
            Scene* scene = &sceneIter->second;
            try
            {
                DeserializeToGuid(_nextSceneGuid);
                scene->_isLoaded = true;
                scene->_isDirty  = false;
                _lodedSceneList.push_back(scene);
            }
            catch (const std::exception& ex)
            {
                std::string sceneName = scene->Name;
                std::string msg       = std::format("{}{}{}", sceneName, (const char*)u8" 로드 실패. ", ex.what());
                UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
            }
        }
        _nextSceneGuid.clear();
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
    auto& [UpdateSet, UpdateQueue] = _updateEnableQueue;
    for (auto& value : OnEnableValue)
    {
        *value = true;  
    }
      
    for (auto& object : UpdateQueue)
    {
        GameObject::Engine::UpdateActiveInHierarchy(object);
    }

    for (auto& component : OnEnableVec)
    {
        component->UpdateEnableInHierarchy();
        if (_isPlay)
        {
            component->OnEnable();
        }
    }
    
    OnEnableSet.clear();
    OnEnableVec.clear();
    OnEnableValue.clear();
    UpdateQueue.clear();
    UpdateSet.clear();
    UpdateQueue.clear();
}

void ESceneManager::ObjectsOnDisable()
{
    auto& [OnDisableSet, OnDisableVec, OnDisableValue] = _onDisableQueue;
    auto& [UpdateSet, UpdateQueue] = _updateDisableQueue;

    for (auto& value : OnDisableValue)
    {
        *value = false;
    }

    for (auto& object : UpdateQueue)
    {
        GameObject::Engine::UpdateActiveInHierarchy(object);
    }

    for (auto& component : OnDisableVec)
    {
        component->UpdateEnableInHierarchy();
        if (_isPlay)
        {
            component->OnDisable();
        }
    }
    
    OnDisableSet.clear();
    OnDisableVec.clear();
    OnDisableValue.clear();
    UpdateSet.clear();
    UpdateQueue.clear();
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
            if (destroyComponent->_gameObject->ActiveInHierarchy_property_getter())
            {

                if (destroyComponent->Enable)
                {
                    destroyComponent->OnDestroy();
                }
            }
        }

        //해당 컴포넌트를 오브젝트 배열에서 삭제.
        std::vector<std::shared_ptr<Component>>& components = destroyComponent->_gameObject->_components;
        std::erase_if(
            components, 
            [destroyComponent](std::shared_ptr<Component>& component)
            {     
                return destroyComponent == component.get();
            }
        );

        NotInitDestroyComponentEraseToWaitVec(destroyComponent);
    }

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
        if (STR_NULL != destroyObject->_ownerScene)
        {
            std::shared_ptr<GameObject>& pObject = _runtimeObjects[instanceID];
            SetObjectOwnerScene(pObject.get(), STR_NULL);
            EraseGameObjectMap(pObject);
            pObject.reset();
        }
    }

    //배열 정리
    while (_runtimeObjects.empty() == false && _runtimeObjects.back() == nullptr)
    {
        _runtimeObjects.pop_back();
    }

    //파괴 큐 초기화
    destroyComponentSet.clear();
    destroyComponentQueue.clear();
    destroyObjectSet.clear();
    destroyObjectQueue.clear();
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
        GameObject::Engine::UpdateActiveInHierarchy(gameObject.get());     
    }
    _addGameObjectsQueue.clear();

    for (auto& component : _addComponentsQueue)
    {
        if (_isPlay)
        {
            _waitAwakeVec.push_back(component);
            _waitStartVec.push_back(component);
        }
        if (component->_type == Component::TYPE::CAMERA)
        {
            component->gameObject->_transform._hasChanged = true;
        }
        component->UpdateEnableInHierarchy();
    }
    _addComponentsQueue.clear();
}

bool ESceneManager::IsRuntimeActive(std::shared_ptr<GameObject>& obj)
{
    return obj.get() != nullptr && obj->ActiveInHierarchy_property_getter() && obj->IsValid();
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
    if (component->gameObject->IsValid())
    {
        auto& [set, vec]    = engineCore->SceneManager._destroyComponentsQueue;
        auto [iter, result] = set.insert(component);
        if (result)
        {
            vec.push_back(component);
        }
    }
}

void ESceneManager::SetObjectOwnerScene(GameObject* object, std::string_view sceneName) 
{
    object->_ownerScene = sceneName;
}

void ESceneManager::SetRendererSkyBox(Scene* scene) 
{
    // 스카이 박스 로드
    if (STR_NULL != scene->_skyBox)
    {
        bool loadSkyBox = false;
        if (STR_NULL != _prevScene)
        {
            File::Guid prevGuid = UmFileSystem.GetGuidFromPath(_prevScene);
            if (false == prevGuid.IsNull())
            {
                Scene& prevSccene = _scenesMap[prevGuid];
                if (prevSccene._skyBox != scene->_skyBox)
                {
                    loadSkyBox = true;
                }
            }
            else
            {
                loadSkyBox = true;
            }
        }
        else
        {
            loadSkyBox = true;
        }

        if (loadSkyBox)
        {
            File::Path path = scene->_skyBox.ToPath();
            if (false == path.IsNull())
            {
                UmGraphics.SetSkyBox("Game", path.c_str());

                if constexpr (IS_EDITOR)
                {
                    UmGraphics.SetSkyBox("Editor", path.c_str());
                }
            }
        }
    }
    else
    {
        UmGraphics.ResetSkyBox("Game");
        if constexpr (IS_EDITOR)
        {
            UmGraphics.ResetSkyBox("Editor");
        }
    }
}

void ESceneManager::AddDestroyObjectQueue(GameObject* gameObject) 
{
    if (gameObject->IsValid())
    {
        auto& [set, vec] = engineCore->SceneManager._destroyObjectsQueue;
        Transform::ForeachDFS(gameObject->_transform, [&set, &vec](Transform* pTransform) {
            auto [iter, result] = set.insert(&pTransform->gameObject);
            if (result)
            {
                vec.push_back(&pTransform->gameObject);
            }
        });
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

    if (false == targetScene._skyBox.ToPath().IsNull())
    {
        sceneNode["SkyBox"] = targetScene._skyBox.string();
    }

    auto rootObjects = scene.GetRootGameObjects();
    for (auto& object : rootObjects)
    {
        YAML::Node objectNode = UmGameObjectFactory.SerializeToYaml(object.get(), true);
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

    const YAML::Node& rootObjects = sceneNode["GameObjects"];
    for (auto object : rootObjects)
    {
        const YAML::Node& objectNodes = object;
        YAML::Node rootObjectNode = *objectNodes.begin();

        if (false == rootObjectNode.IsNull())
        {
            std::shared_ptr<GameObject> newObject = UmGameObjectFactory.DeserializeToSceneObject(object);
            if (nullptr == newObject)
            {
                UmLogger.Log(LogLevel::LEVEL_WARNING, u8"오브젝트 역직렬화 실패. 씬 파일에 누락된 오브젝트가 존재합니다."_c_str);
                continue;
            }

            Transform::ForeachDFS(newObject->_transform, [&Guid](Transform* curr) 
            { 
                curr->_gameObject._ownerScene = Guid.ToPath().string(); 
            });
        }
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
    bool result = WriteUmSceneFile(scene, name, outPath, isOverride, true);
}

bool ESceneManager::SetSkyBox(const File::Path& path)
{
    Scene* mainScene = GetMainScene();
    if (nullptr == mainScene)
    {
        return false;
    }

    File::Guid guid = path.ToGuid();
    if (true == guid.IsNull())
    {
        return false;
    }

    Engine::SetSceneSkyBoxGuid(*mainScene, guid);
    UmGraphics.SetSkyBox("Game", path.c_str());

    if constexpr (IS_EDITOR)
    {
        UmGraphics.SetSkyBox("Editor", path.c_str());
    }

    mainScene->IsDirty = true;

    return true;
}

const std::vector<std::weak_ptr<MeshComponent>>& ESceneManager::GetMeshComponents()
{
    std::erase_if(_runtimeMeshComponents, [](const std::weak_ptr<MeshComponent>& weakMesh) 
    { 
        return weakMesh.expired();
    });

    return _runtimeMeshComponents;
}

bool ESceneManager::WriteUmSceneFile(Scene& scene, std::string_view sceneName, std::string_view outPath, bool isOverride, bool isEmptyScene)
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
    bool isEmptyOk = isEmptyScene ? true : (bool)node["GameObjects"]; 
    if (node.IsNull() == false && isEmptyOk)
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
    const auto& [node, result] = YAMLHelper::SafeLoadFile(path);
    if (result)
    {
        try
        {
            _sceneDataMap[guid] = node;
            YAML::Node& sceneNode = _sceneDataMap[guid];
            Scene& scene = _scenesMap[guid];
            scene._guid  = guid;
            _scenesFindMap[scene.Name].insert(guid);
            if (sceneNode["SkyBox"])
            {
                scene._skyBox = sceneNode["SkyBox"].as<std::string>();
            }
            std::string nodeGuid = sceneNode["Guid"].as<std::string>();
            if (nodeGuid != guid)
            {
                sceneNode["Guid"] = guid.string();
                if (node.IsNull() == false)
                {
                    std::ofstream ofs(path, std::ios::trunc);
                    if (ofs.is_open())
                    {
                        ofs << sceneNode;
                    }
                    ofs.close();
                }
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
        catch (const YAML::BadConversion& ex)
        {
            std::string msg =
                std::format("{}{} {}", (const char*)u8"올바르지 않은 UmScene 파일입니다. ", path.string(), ex.what());
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
            EraseSceneGUID((std::string)_scenesMap[guid].Name, guid);
        }
        catch (const YAML::Exception& ex)
        {
            std::string msg =
                std::format("{}{} {}", (const char*)u8"올바르지 않은 UmScene 파일입니다. ", path.string(), ex.what());
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
            EraseSceneGUID((std::string)_scenesMap[guid].Name, guid);
        }
    }
    else
    {
        std::string msg = std::format("{}{} {}",  (const char*)u8"올바르지 않은 UmScene 파일입니다. ", path.string(), result.What());
        UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        EraseSceneGUID((std::string)_scenesMap[guid].Name, guid);
    }
}

void ESceneManager::OnFileUnregistered(const File::Path& path) 
{
    File::Guid guid = path.ToGuid();
    Scene& scene = _scenesMap[guid];
    std::string sceneName = scene.Name;
    EraseSceneGUID(sceneName, guid);
}

void ESceneManager::OnFileModified(const File::Path& path)
{
    File::Guid guid = path.ToGuid();
    const auto& [node, result] = YAMLHelper::SafeLoadFile(path);
    if (result)
    {
        try
        {
            _sceneDataMap[guid]     = node;
            const YAML::Node& node  = _sceneDataMap[guid];
            Scene&            scene = _scenesMap[guid];
            scene._guid             = guid;
            if (node["SkyBox"])
            {
                scene._skyBox = node["SkyBox"].as<std::string>();
            }
        }
        catch (const YAML::BadConversion& ex)
        {
            std::string msg =
                std::format("{}{} {}", (const char*)u8"올바르지 않은 UmScene 파일입니다. ", path.string(), ex.what());
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
            EraseSceneGUID((std::string)_scenesMap[guid].Name, guid);
        }
        catch (const YAML::Exception& ex)
        {
            std::string msg =
                std::format("{}{} {}", (const char*)u8"올바르지 않은 UmScene 파일입니다. ", path.string(), ex.what());
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
            EraseSceneGUID((std::string)_scenesMap[guid].Name, guid);
        }
    }
    else
    {
        std::string msg = std::format("{}{} {}",  (const char*)u8"올바르지 않은 UmScene 파일입니다. ", path.string(), result.What());
        UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        EraseSceneGUID((std::string)_scenesMap[guid].Name, guid);
    }
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
    if (_scenesMap.find(guid) != _scenesMap.end())
    {
        Scene* pScene  = &_scenesMap[guid];
        pScene->_isLoaded = false;
        if (_setting.MainScene == sceneName)
        {
            _setting.MainScene = STR_NULL;
        }
        auto   objects = pScene->GetRootGameObjects();
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
}

template <typename T>
void ESceneManager::SceneResourceManager::UpdateRenderResource(RenderResource<T>& resource)
{
    std::tuple<std::weak_ptr<Component>, File::Path, std::function<void()>> curr;
    while (false == resource.ResourceLoadQueue.empty())
    {
        if (true == resource.ResourceLoadQueue.try_pop(curr))
        {
            auto& [weakPtr, path, func] = curr;
            if (std::shared_ptr<Component> component = weakPtr.lock())
            {
                if (true == std::filesystem::exists(path))
                {
                    if (component->_gameObject->IsValid())
                    {
                        path = std::filesystem::absolute(path);
                        path = path.generic_string();
                        auto findIter = resource.RenderResource.find(path);
                        if (findIter == resource.RenderResource.end())
                        {
                            auto newResource = UmResourceManager->LoadResource<T>(path.string());                       
                            resource.RenderResource[path] = newResource;
                        }
                        func();
                    }
                }
                else
                {
                    std::string_view componentName = component->ClassName();
                    const std::string& objectName    = component->gameObject->Name;
                    std::string      msg =
                        std::format("{}{}{} {}", path.string(), (const char*)u8"는 존재하지 않는 리소스입니다. ",
                                    objectName, componentName);
                    UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
                                
                }
            }
        }
    }
}

void ESceneManager::SceneResourceManager::Engine::Update(SceneResourceManager& manager)
{
    manager.UpdateRenderResource(manager._models);
    manager.UpdateRenderResource(manager._textures);
    manager.UpdateRenderResource(manager._fonts);
}

void ESceneManager::SceneResourceManager::RequestModelResource(const Component* component, const File::Guid& guid,
                                                               const std::function<void()> func)
{
    if (component->gameObject->IsValid())
    {
        File::Path path = UmFileSystem.GetPathFromGuid(guid);
        if (false == path.IsNull())
        {
            RequestModelResource(component, path, func);
        }
        else
        {
            std::string_view componentName = component->ClassName();
            const std::string& objectName    = component->gameObject->Name;
            std::string msg = std::format("{}{}{} {}", guid.string(), (const char*)u8"는 존재하지 않는 리소스입니다. ",
                                          objectName, componentName);
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        }
    }
}

void ESceneManager::SceneResourceManager::RequestModelResource(const Component* component, const File::Path& path,
                                                               const std::function<void()> func)
{
    if (component->gameObject->IsValid())
    {
        if (true == std::filesystem::exists(path))
        {
            auto tuple = std::make_tuple(component->GetWeakPtr(), path, func);
            _models.ResourceLoadQueue.push(tuple);
        }
        else
        {
            std::string_view componentName = component->ClassName();
            const std::string& objectName    = component->gameObject->Name;
            std::string msg = std::format("{}{}{} {}", path.string(), (const char*)u8"는 존재하지 않는 리소스입니다. ",
                                          objectName, componentName);
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        }
    }
}

void ESceneManager::SceneResourceManager::RequestTextureResource(const Component* component, const File::Guid& guid, const std::function<void()> func)
{
    if (component->gameObject->IsValid())
    {
        File::Path path = UmFileSystem.GetPathFromGuid(guid);
        if (false == path.IsNull())
        {
            RequestTextureResource(component, path, func);
        }
        else
        {
            std::string_view componentName = component->ClassName();
            const std::string& objectName    = component->gameObject->Name;
            std::string msg = std::format("{}{}{} {}", guid.string(), (const char*)u8"는 존재하지 않는 리소스입니다. ",
                                          objectName, componentName);
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        }
    }
}

void ESceneManager::SceneResourceManager::RequestTextureResource(const Component* component, const File::Path& path,
                                                                 const std::function<void()> func)
{
    if (component->gameObject->IsValid())
    {
        if (true == std::filesystem::exists(path))
        {
            auto tuple = std::make_tuple(component->GetWeakPtr(), path, func);
            _textures.ResourceLoadQueue.push(tuple);
        }
        else
        {
            std::string_view componentName = component->ClassName();
            const std::string& objectName    = component->gameObject->Name;
            std::string msg = std::format("{}{}{} {}", path.string(), (const char*)u8"는 존재하지 않는 리소스입니다. ",
                                          objectName, componentName);
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        }
    }
}

void ESceneManager::SceneResourceManager::RequestFontResource(const Component* component, const File::Guid& guid,
                                                              const std::function<void()> func)
{
    if (component->gameObject->IsValid())
    {
        File::Path path = UmFileSystem.GetPathFromGuid(guid);
        if (false == path.IsNull())
        {
            RequestFontResource(component, path, func);
        }
        else
        {
            std::string_view componentName = component->ClassName();
            const std::string& objectName    = component->gameObject->Name;
            std::string msg = std::format("{}{}{} {}", guid.string(), (const char*)u8"는 존재하지 않는 리소스입니다. ",
                                          objectName, componentName);
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        }
    }
}

void ESceneManager::SceneResourceManager::RequestFontResource(const Component* component, const File::Path& path,
                                                              const std::function<void()> func)
{
    if (component->gameObject->IsValid())
    {
        if (true == std::filesystem::exists(path))
        {
            auto tuple = std::make_tuple(component->GetWeakPtr(), path, func);
            _fonts.ResourceLoadQueue.push(tuple);
        }
        else
        {
            std::string_view componentName = component->ClassName();
            const std::string& objectName    = component->gameObject->Name;
            std::string msg = std::format("{}{}{} {}", path.string(), (const char*)u8"는 존재하지 않는 리소스입니다. ",
                                          objectName, componentName);
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        }
    }
}

ESceneManager::SceneResourceManager::SceneResourceManager() 
{

}

ESceneManager::SceneResourceManager::~SceneResourceManager() 
{

}

void ESceneManager::InputSystem::UpdateInput()
{
    if (false == _isConnect)
    {
        static float elapsedTime = 0.f;
        elapsedTime += UmTime.UnscaledDeltaTime();
        if (elapsedTime >= 0.2f)
        {
            try
            {
                _inputController.Connect();
                _isConnect = true;
                UmLogger.Log(LogLevel::LEVEL_DEBUG, (const char*)u8"컨트롤러 연결됨!");
            }
            catch (...)
            {
                // 예외 처리: 컨트롤러 연결 실패
            }
            elapsedTime = 0.f;
        }
    }
    else
    {
        try
        {
            _inputController.UpdateState();
            const auto& queue = _inputController.GetButtonQueue();
            if (false == queue.empty())
            {
                for (const auto& flag : queue)
                {
                    UpdateTracker(flag);
                }
            }        
            
            UpdateAnalogButtons();
            std::memset(_actionChecker.data(), 0, std::size(_actionChecker)); //중복 액션 방지용 기록 배열 초기화.
        }
        catch (const Input::DeviceNotConnectedException& exception)
        {
            UmLogger.Log(LogLevel::LEVEL_DEBUG, exception.what());
            UmLogger.Log(LogLevel::LEVEL_DEBUG, (const char*)u8"컨트롤러 해제됨!");
            _isConnect = false;
        }
        catch (const Input::InputException& exception)
        {
            UmLogger.Log(LogLevel::LEVEL_DEBUG, exception.what());
#ifdef _DEBUG
            throw;
#endif
        }

    }
}

void ESceneManager::InputSystem::UpdateTracker(Input::Controller::Button button)
{
    int   buttonIndex = std::countr_zero((unsigned int)button);
    bool& checker     = _actionChecker[buttonIndex];
    if (checker)
    {
        return;
    }
    Action& action = _actionTracker[buttonIndex];
    bool    isDown = false;
    switch (button)
    {
    case Input::Controller::Button::DPAD_UP:
    case Input::Controller::Button::DPAD_DOWN:
    case Input::Controller::Button::DPAD_LEFT:
    case Input::Controller::Button::DPAD_RIGHT:
    case Input::Controller::Button::START:
    case Input::Controller::Button::BACK:
    case Input::Controller::Button::LEFT_THUMB_BUTTON:
    case Input::Controller::Button::RIGHT_THUMB_BUTTON:
    case Input::Controller::Button::LEFT_SHOULDER:
    case Input::Controller::Button::RIGHT_SHOULDER:
    case Input::Controller::Button::A:
    case Input::Controller::Button::B:
    case Input::Controller::Button::X:
    case Input::Controller::Button::Y:
        isDown = _inputController.IsButtonDown(button);
        break;
    case Input::Controller::Button::LEFT_THUMB_STICK:
        isDown = 0.f < _inputController.GetLeftThumbStickAxis().Magnitude;
        break;
    case Input::Controller::Button::RIGHT_THUMB_STICK:
        isDown = 0.f < _inputController.GetRightThumbStickAxis().Magnitude;
        break;
    case Input::Controller::Button::LEFT_TRIGGER:
        isDown = 0.f < _inputController.GetLeftTrigger();
        break;
    case Input::Controller::Button::RIGHT_TRIGGER:
        isDown = 0.f < _inputController.GetRightTrigger();
        break;
    default:
        break;
    }

    if (true == isDown)
    {
        switch (action)
        {
        case ESceneManager::InputSystem::Action::PRESSED:
            action = Action::HELD;
            break;
        case ESceneManager::InputSystem::Action::RELEASED:
        case ESceneManager::InputSystem::Action::IDLE:
            action = Action::PRESSED;         
            break;
        default:
            break;
        }
    }
    else
    {
        switch (action)
        {
        case ESceneManager::InputSystem::Action::PRESSED:
        case ESceneManager::InputSystem::Action::HELD:
            action = Action::RELEASED;
            break;
        case ESceneManager::InputSystem::Action::RELEASED:
            action = Action::IDLE;
            break;
        default:
            break;
        }
    }

    int   actionIndex = static_cast<int>(action);
    auto& receivers = _receivers[buttonIndex][actionIndex];
    for (auto& [instance, event] : receivers)
    {
        event(_inputController);
        checker = true;
    }
}

void ESceneManager::InputSystem::UpdateAnalogButtons() 
{
    // 아날로그 버튼들은 항상 갱신 필요
    constexpr int leftTriggerIndex  = std::countr_zero((unsigned int)Input::Controller::Button::LEFT_TRIGGER);
    constexpr int rightTriggerIndex = std::countr_zero((unsigned int)Input::Controller::Button::RIGHT_TRIGGER);
    constexpr int leftThumbIndex    = std::countr_zero((unsigned int)Input::Controller::Button::LEFT_THUMB_STICK);
    constexpr int rightThumbIndex   = std::countr_zero((unsigned int)Input::Controller::Button::RIGHT_THUMB_STICK);

    if (_actionTracker[leftTriggerIndex] == Action::HELD)
        UpdateTracker(Input::Controller::Button::LEFT_TRIGGER);

    if (_actionTracker[rightTriggerIndex] == Action::HELD)
        UpdateTracker(Input::Controller::Button::RIGHT_TRIGGER);

    if (_actionTracker[leftThumbIndex] == Action::HELD)
        UpdateTracker(Input::Controller::Button::LEFT_THUMB_STICK);

    if (_actionTracker[rightThumbIndex] == Action::HELD)
        UpdateTracker(Input::Controller::Button::RIGHT_THUMB_STICK);
}
