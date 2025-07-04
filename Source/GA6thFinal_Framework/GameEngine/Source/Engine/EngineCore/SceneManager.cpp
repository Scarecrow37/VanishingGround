#include "pch.h"
#include "Engine/GraphicsCore/Model.h"
#include "Engine/GraphicsCore/Light.h"
#include "Engine/GraphicsCore/Animator.h"
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

void ESceneManager::Engine::SwapPrefabInstance(GameObject* original, GameObject* remake)
{
    ESceneManager& sceneManager = UmSceneManager;
    int index = original->GetInstanceID();
    std::shared_ptr<GameObject>& sOrigin = sceneManager._runtimeObjects[index];
    if (nullptr != sOrigin)
    {
        std::shared_ptr<GameObject> sRemake = remake->GetWeakPtr().lock();
        std::swap(sOrigin->_instanceID, sRemake->_instanceID);
        std::swap(sOrigin->_ownerScene, sRemake->_ownerScene);
        std::swap(sOrigin, sRemake);
        std::string objectData = sRemake->SerializedReflectFields();
        sOrigin->DeserializedReflectFields(objectData);
        sOrigin->_transform = sRemake->_transform;
        sceneManager.EraseGameObjectMap(sRemake);
        sceneManager.InsertGameObjectMap(sOrigin);
        GameObject::Engine::UpdateActiveInHierarchy(sOrigin.get());

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
        }
        _setting.MainScene = scene->Path;
        _addComponentsQueue.clear();
        _addGameObjectsQueue.clear();
        _lodedSceneList.clear();
        UmCommandManager.Clear();
        SetRendererSkyBox(scene);
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

    DeserializeToGuid(scene->_guid);
    scene->_isLoaded = true;
    scene->_isDirty  = false;
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
    if (nullptr != _mainCamera)
    {
        if (true == _mainCamera->IsDirty())
        {
            _mainCamera->UpdatePerspective();
        }
        
        Transform* root = _mainCamera->gameObject->transform->Root;
        Transform& transform = root ? *root : _mainCamera->gameObject->transform;
        if (true == transform._hasChanged)
        {
            transform.UpdateMatrix();
            _mainCamera->UpdateView();
        }
    }

    static std::unordered_set<Transform*> updateCheckSet;
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
        if (STR_NULL != destroyObject->_ownerScene)
        {
            std::shared_ptr<GameObject>& pObject = _runtimeObjects[instanceID];
            SetObjectOwnerScene(pObject.get(), STR_NULL);
            EraseGameObjectMap(pObject);
            pObject.reset();
        }
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
        GameObject::Engine::ResetActiveInHierarchy(gameObject.get());     
    }
    _addGameObjectsQueue.clear();

    for (auto& component : _addComponentsQueue)
    {
        component->_gameObject->_components.emplace_back(component);
        if (_isPlay)
        {
            _waitAwakeVec.push_back(component);
            _waitStartVec.push_back(component);
        }

        if (component->_type == Component::TYPE::CAMERA)
        {
            CameraComponent* camera = static_cast<CameraComponent*>(component.get());
            std::shared_ptr<Camera> newCamera(new Camera);
            camera->SetTarget(newCamera);
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
                UmRenderer.SetSkyBox(path.string());
            }
        }
    }
    else
    {
        UmRenderer.ResetSkyBox();
    }
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

    if (false == targetScene._skyBox.ToPath().IsNull())
    {
        sceneNode["SkyBox"] = targetScene._skyBox.string();
    }

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
                UmLogger.Log(LogLevel::LEVEL_FATAL, u8"메모리 할당 실패."_c_str);
                __debugbreak();
                UmApplication.Quit();
                return false;
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
    bool result = WriteUmSceneFile(scene, name, outPath, isOverride);
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
    UmRenderer.SetSkyBox(path.string());
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

    Scene& scene = _scenesMap[guid];
    scene._guid  = guid;
    if (node["SkyBox"])
    {
        scene._skyBox = node["SkyBox"].as<std::string>();
    }
    _scenesFindMap[scene.Name].insert(guid);
    std::string nodeGuid = node["Guid"].as<std::string>();
    if (nodeGuid != guid)
    {
        node["Guid"] = guid.string();
        if (node.IsNull() == false)
        {
            std::ofstream ofs(path, std::ios::trunc);
            if (ofs.is_open())
            {
                ofs << node;
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
    _sceneDataMap[guid] = YAML::LoadFile(path.string());
    const YAML::Node& node = _sceneDataMap[guid];
    Scene& scene = _scenesMap[guid];
    scene._guid  = guid;
    if (node["SkyBox"])
    {
        scene._skyBox = node["SkyBox"].as<std::string>();
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
                            if (0 <= pMeshComponent->_gameObject->_instanceID)
                            {
                                if (models.ModelResource.find(guid) == models.ModelResource.end())
                                {
                                    models.ModelResource[guid] = UmResourceManager.LoadResource<Model>(path.string());
                                }
                                meshRenderer.LoadModel(path.wstring());
                                models.ModelUseComponentList[guid].emplace_back(pMeshComponent);
                                UmSceneManager._runtimeMeshComponents.emplace_back(pMeshComponent);
                                auto& animation = meshRenderer.GetModel()->GetAnimation();
                                auto& skeleton  = meshRenderer.GetModel()->GetSkeleton();
                                if (animation != nullptr && skeleton != nullptr)
                                {
                                    std::shared_ptr<Animator> animator(new Animator);
                                    animator->Initialize(animation, skeleton);
                                    animator->SetActive(&pMeshComponent->EnableInHierarchy);
                                    meshRenderer.SetAnimator(animator);
                                    UmAnimationCore.RegisterAnimator(animator.get());
                                }
                            }
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
    File::Path path = UmFileSystem.GetPathFromGuid(guid);
    if (false == path.IsNull())
    {
        if (auto sharedPtr = meshComponent->GetWeakPtr().lock())
        {
            std::weak_ptr<MeshComponent> weakPtr = std::static_pointer_cast<MeshComponent>(sharedPtr);
            auto                         pair    = std::make_pair(weakPtr, guid);
            _models.ModelLoadQueue.push(pair);
        }
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, std::format("{}{}", guid.string(), u8"는 존재하지 않는 리소스입니다."_c_str));
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

            for (int buttonIndex = 0; buttonIndex < _receivers.size(); ++buttonIndex)
            {
                auto& buttons = _receivers[buttonIndex];
                for (int actionIndex = 0; actionIndex < buttons.size(); ++actionIndex)
                {
                    Action action  = (Action)actionIndex;
                    auto&  actions = buttons[actionIndex];
                    for (auto& [component, event] : actions)
                    {
                        Action& actionTracker = _actionTracker[buttonIndex];
                        if (action == actionTracker)
                        {
                            event(_inputController);                       
                        }

                        switch (actionTracker)
                        {
                        case Action::PRESSED:
                            actionTracker = Action::HELD;
                            break;
                        case Action::RELEASED:
                            actionTracker = Action::IDLE;
                        default:
                            break;
                        }

                    }
                }
            }
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
    int index = std::countr_zero((unsigned int)button); 
    Action& action = _actionTracker[index];
    bool    isDown = false;

    switch (button)
    {
    case Input::Controller::DPAD_UP:
    case Input::Controller::DPAD_DOWN:
    case Input::Controller::DPAD_LEFT:
    case Input::Controller::DPAD_RIGHT:
    case Input::Controller::START:
    case Input::Controller::BACK:
    case Input::Controller::LEFT_THUMB_BUTTON:
    case Input::Controller::RIGHT_THUMB_BUTTON:
    case Input::Controller::LEFT_SHOULDER:
    case Input::Controller::RIGHT_SHOULDER:
    case Input::Controller::A:
    case Input::Controller::B:
    case Input::Controller::X:
    case Input::Controller::Y:
        isDown = _inputController.IsButtonDown(button);
        break;
    case Input::Controller::LEFT_THUMB_STICK:
        isDown = 0.f < _inputController.GetLeftThumbStickAxis().Magnitude;
        break;
    case Input::Controller::RIGHT_THUMB_STICK:
        isDown = 0.f < _inputController.GetRightThumbStickAxis().Magnitude;
        break;
    case Input::Controller::LEFT_TRIGGER:
        isDown = 0.f < _inputController.GetLeftTrigger();
        break;
    case Input::Controller::RIGHT_TRIGGER:
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

}
