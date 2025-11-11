#include "pch.h"
#include "Camera/CameraComponent.h"
#include "Mesh/MeshComponent.h"
#include "GraphicsEngine/SDFFont.h"

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
    : 
    _mainCamera(nullptr), 
    _nextSceneSkybox(nullptr)
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
    auto& [enableSet, enableVec] = engineCore->SceneManager._onEnableQueue;
    enableSet.clear();
    enableVec.clear();

    // OnDisable 큐
    auto& [disableSet, disableVec] = engineCore->SceneManager._onDisableQueue;
    disableSet.clear();
    disableVec.clear();

    //Render component들
    engineCore->SceneManager._runtimeMeshComponents.clear();

    //리소스
    SceneResourceManager::Engine::CleanUp(engineCore->SceneManager.ResourceManager);

    //인풋
    engineCore->SceneManager._inputSystem.CleanupInputReceivers();
}

void ESceneManager::Engine::SceneUpdate()
{
    engineCore->SceneManager.SceneUpdate();
}

void ESceneManager::Engine::SceneFinalUpdate() 
{
    engineCore->SceneManager.SceneFinalUpdate();
}

void ESceneManager::SceneUpdate()
{
    /*
    if (ResourceLoadWait())
    {
        return;
    }
    */

    ObjectsInputUpdate();                                // Input System 콜백은 항상 업데이트 주기보다 먼저 실행됨.
    while (ETimeSystem::Engine::TimeSystemFixedUpdate()) // Fixed Update는 한 프레임에 여러번 호출 가능함
    {
        ObjectsFixedUpdate();   
    }
    ObjectsUpdate();                                     // 메인 로직 업데이트
    ObjectsLateUpdate();                                 // 두번째 로직 업데이트

    //로직 업데이트 이후 요청된 라이프 사이클들은 아래에서 반드시 실행 (이번 프레임에 바로 처리되야함)
    ObjectsAddRuntime();              
    ResourceManagerUpdate();
    ObjectsOnEnable();
    ObjectsOnDisable();
    ObjectsAwake();
    ObjectsStart();
    ObjectsApplicationQuit();
    ObjectsDestroy();
    ObjectsAddLoadScene();
    ObjectsMatrixUpdate();
}

void ESceneManager::SceneFinalUpdate() 
{
    //그래픽스 Flip 이후 실행해야할 로직들
    ObjectsTransformFlagReset();
    ObjectsPrevFrameEnableUpdate();
#ifdef _UMEDITOR
    _isPlay = editorModule->PlayMode.IsPlay(); // 플레이 갱신은 마지막에 해야함.
#endif
}

void ESceneManager::ObjectsTransformFlagReset() 
{
    for (auto& obj : _runtimeObjects)
    {
        if (nullptr != obj && obj->_transform._hasChanged == true)
        {
            obj->_transform._hasChanged = false;
        }
    }
}

void ESceneManager::ObjectsPrevFrameEnableUpdate()
{
    for (auto& obj : _runtimeObjects)
    {
        if (nullptr != obj)
        {
            for (auto& component : obj->_components)
            {
                component->_prevFrameEnableInHierarchy = component->_enableInHierarchy;
            }
        }
    }
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
    Global::engineCore->SceneManager._addComponentsQueue.emplace_back(component->_gameObject->GetWeakPtr(), component);
    EComponentFactory::Engine::PushBackComponentToObject(component);
    component->Reset();
}

void ESceneManager::Engine::SetGameObjectActive(GameObject* pObject, bool value)
{
    ESceneManager& sceneManager = UmSceneManager;
    if (nullptr != pObject)
    {
        GameObject* gameObject = pObject;
        if (gameObject->ReflectFields->_activeSelf != value)
        {
            // true로 변경시 값 적용 후 대기열 추가
            if (true == value)
            {            
                gameObject->ReflectFields->_activeSelf = value;
                GameObject::Engine::UpdateActiveInHierarchy(gameObject);
            }
            
            // 컴포넌트들의 On__able 함수를 호출하도록 합니다.
            auto& [WaitSet, WaitVec] = value ? sceneManager._onEnableQueue : sceneManager._onDisableQueue;
            Transform::ForeachDFS(gameObject->_transform, 
            [&](Transform* curr) 
            {
                if (curr->gameObject->_activeInHierarchy == true)
                {
                    for (auto& component : curr->gameObject->_components)
                    {
                        if (true == component->ReflectFields->_enable)
                        {
                            if (component->_initFlags.IsAwake() == true)
                            {
                                if (component->_prevFrameEnableInHierarchy != value)
                                {
                                    auto [iter, result] = WaitSet.insert(component.get());
                                    if (result)
                                    {
                                        WaitVec.push_back(component);
                                    }
                                }
                            }
                        }
                    }
                }                   
            });  
            
            // false로 변경시 대기열 추가 후 값 적용
            if (false == value)
            {
                gameObject->ReflectFields->_activeSelf = value;
                GameObject::Engine::UpdateActiveInHierarchy(gameObject);
            }

            // 기존에 변경요청을 했으면 On__able 함수 대기열에서 제거
            auto& [notWaitSet, notWaitVec] = value ? sceneManager._onDisableQueue : sceneManager._onEnableQueue;
            Transform::ForeachDFS(gameObject->_transform, [&](Transform* curr) 
            {
                // vector 대기열에서 제거
                std::erase_if(notWaitVec, [&notWaitSet](const std::weak_ptr<Component>& weak) 
                {
                    if (auto component = weak.lock())
                    {
                        return notWaitSet.find(component.get()) != notWaitSet.end();
                    }
                    return true;
                });

                // set에서 제거
                for (auto& component : curr->gameObject->_components)
                {
                    if (component->_initFlags.IsAwake() == true)
                    {
                        if (auto findIter = notWaitSet.find(component.get()); findIter != notWaitSet.end())
                        {
                            notWaitSet.erase(findIter);
                        }
                    }
                }
            });

        }
    }    
}

void ESceneManager::Engine::SetComponentEnable(Component* component, bool value)
{
    ESceneManager& sceneManager = UmSceneManager;
    if (component)
    {
        if (component->ReflectFields->_enable != value)
        {        
            // true로 변경시 값 적용 후 대기열 추가
            if (true == value)
            {
                component->ReflectFields->_enable = value;
                component->UpdateEnableInHierarchy();
            }

            // 컴포넌트의 On__able 함수를 호출하도록 대기열에 추가
            auto& [WaitSet, WaitVec] = value ? sceneManager._onEnableQueue : sceneManager._onDisableQueue;
            if (component->_enableInHierarchy == true)
            {
                if (component->_prevFrameEnableInHierarchy != value)
                {
                    auto [iter, result] = WaitSet.insert(component);
                    if (result)
                    {
                        WaitVec.push_back(component->GetWeakPtr());
                    }
                }
            }

            // false로 변경시 대기열 추가 후 값 적용
            if (false == value)
            {
                component->ReflectFields->_enable = value;
                component->UpdateEnableInHierarchy();
            }

            // 기존에 변경요청을 했으면 On__able 함수 대기열에서 제거
            auto& [notWaitSet, notWaitVec] = value ? sceneManager._onDisableQueue : sceneManager._onEnableQueue;
            if (auto findIter = notWaitSet.find(component); findIter != notWaitSet.end())
            {
                notWaitSet.erase(findIter);
                std::erase_if(notWaitVec, [&component](const std::weak_ptr<Component>& weak) 
                {
                    if (auto vecComponent = weak.lock())
                    {
                        return vecComponent.get() == component;
                    }
                    return true;
                });
            }
        }
    } 
}

std::weak_ptr<GameObject> ESceneManager::Engine::FindGameObjectWithName(const std::string& name)
{
    std::weak_ptr<GameObject> findObject;
    auto findIter = engineCore->SceneManager._runtimeObjectsUnorderedMap.find(name);
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

std::vector<std::weak_ptr<GameObject>> ESceneManager::Engine::FindGameObjectsWithName(const std::string& name)
{
    std::vector<std::weak_ptr<GameObject>> findObjects;
    auto findIter = engineCore->SceneManager._runtimeObjectsUnorderedMap.find(name);
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

std::weak_ptr<GameObject> ESceneManager::Engine::FindGameObjectWithTag(const std::string& tag)
{
    std::weak_ptr<GameObject> findObject;
    auto findIter = engineCore->SceneManager._runtimeObjectsTagMap.find(tag);
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

std::vector<std::weak_ptr<GameObject>> ESceneManager::Engine::FindGameObjectsWithTag(const std::string& tag)
{
    std::vector<std::weak_ptr<GameObject>> findObjects;
    auto findIter = engineCore->SceneManager._runtimeObjectsTagMap.find(tag);
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

        Transform::ForeachBFS(gameObject->_transform, [](Transform * curr)
        {
            curr->gameObject->_ownerScene = DONT_DESTROY_ON_LOAD_SCENE_NAME;
        });   

        if (nullptr != gameObject->transform->Parent)
        {
            Transform* parent = gameObject->transform->Parent;
            if (parent->gameObject->_ownerScene != DONT_DESTROY_ON_LOAD_SCENE_NAME)
            {
                gameObject->transform->SetParent(nullptr);
            }
        }
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
                sRemake->_creationFrame = sOrigin->_creationFrame;
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

void ESceneManager::Engine::SetSceneIBLGuid(Scene& scene, const File::Guid& skyIBL) 
{
    scene._skyIBL = skyIBL;
}

void ESceneManager::Engine::SetSceneIBLPath(Scene& scene, std::string_view skyIBLPath) 
{
    File::Guid guid = UmFileSystem.GetGuidFromPath(skyIBLPath);
    if (false == guid.IsNull())
    {
        SetSceneIBLGuid(scene, guid);
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
    static thread_local std::vector<Component*> onloadSceneTargets;
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
                // 이벤트 호출 대상
                for (auto& component : obj->_components)
                {
                    onloadSceneTargets.push_back(component.get());
                }

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

        //이벤트 호출 대상
        for (auto& obj : _runtimeObjects)
        {
            if (obj)
            {
                for (auto& component : obj->_components)
                {
                    onloadSceneTargets.push_back(component.get());
                }
            }
        }
    }

    // 이벤트 호출
    if (_isPlay)
    {
        for (auto& component : onloadSceneTargets)
        {
            component->OnLoadScene(*scene, mode);
        }
    }
    onloadSceneTargets.clear();
    _nextSceneGuids.push_back(scene->_guid);
    _nextSceneSkybox = scene;
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
    static thread_local std::vector<Component*> awakeVector;
    awakeVector.clear();
    std::ranges::transform(_waitAwakeVec, std::back_inserter(awakeVector), [](const std::shared_ptr<Component>& ptr) { return ptr.get(); });
    for (auto& component : awakeVector)
    {
        if (component->_enableInHierarchy)
        {
            component->Awake();
            component->_initFlags.SetAwake();
            component->OnEnable();
        }
    }
    std::erase_if(_waitAwakeVec, [](auto& component)
    {
        return component->_initFlags.IsAwake();
    });
}

void ESceneManager::ObjectsStart()
{
    static thread_local std::vector<Component*> startVector;
    startVector.clear();
    std::ranges::transform(_waitStartVec, std::back_inserter(startVector), [](const std::shared_ptr<Component>& ptr) { return ptr.get(); });
    for (auto& component : startVector)
    {
        if (component->_enableInHierarchy)
        {
            component->Start();
            component->_initFlags.SetStart();
        }
    }
    std::erase_if(_waitStartVec, [](auto& component)
    { 
        return component->_initFlags.IsStart();
    });
}

bool ESceneManager::ResourceLoadWait()
{
    if (_checkResourceLoad && false == ResourceManager.CheckAllResourceLoad())
    {
        return true;
    }
    else
    {
        _checkResourceLoad = false;       
        return false;
    }
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
    if (false == _nextSceneGuids.empty())
    {
        //안전하게 임시 변수로 이동 후 작업
        std::vector<File::Guid> nextScenes = std::move(_nextSceneGuids);
        _nextSceneGuids.clear();
        for (auto& sceneGuid : nextScenes)
        {
            auto sceneIter = _scenesMap.find(sceneGuid);
            if (sceneIter != _scenesMap.end())
            {
                Scene* scene = &sceneIter->second;
                try
                {
                    DeserializeToGuid(sceneGuid);
                    scene->_isLoaded = true;
                    scene->_isDirty  = false;
                    _lodedSceneList.push_back(scene);
                }
                catch (const YAML::Exception& ex)
                {
                    std::string sceneName = scene->Name;
                    std::string msg       = std::format("{}{}{}", sceneName, (const char*)u8" 로드 실패. ", ex.what());
                    UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
                }
            }
        }
        _waitResourceLoad = true;
    }

    if (nullptr != _nextSceneSkybox)
    {
        SetRendererSkyBox(_nextSceneSkybox);     
        _nextSceneSkybox = nullptr;
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
                        component->OnApplicationQuit();
                    }
                }
            }
        }
    }
}

void ESceneManager::ObjectsOnEnable()
{
    // 컴포넌트들의 이벤트 함수 호출
    auto& [onEnableSet, onEnableVec] = _onEnableQueue;
    static thread_local std::vector<std::shared_ptr<Component>> validComponents;
    validComponents.reserve(onEnableVec.size());

    // 안전하게 복사
    for (auto& weakComponent : onEnableVec)
    {
        if (const auto& component = weakComponent.lock())
        {
            validComponents.push_back(component);
        }
    }
    onEnableSet.clear();
    onEnableVec.clear();

    // 이벤트 호출
    for (auto& component : validComponents)
    {
        if (_isPlay && component->_initFlags.IsAwake())
        {
            component->OnEnable();
        }
    }
    validComponents.clear();
}

void ESceneManager::ObjectsOnDisable()
{
    // 컴포넌트들의 이벤트 함수 호출
    auto& [OnDisableSet, OnDisableVec] = _onDisableQueue;
    static thread_local std::vector<std::shared_ptr<Component>> validComponents;
    validComponents.reserve(OnDisableVec.size());

    // 안전하게 복사
    for (auto& weakComponent : OnDisableVec)
    {
        if (const auto& component = weakComponent.lock())
        {
            validComponents.push_back(component);
        }
    }
    OnDisableSet.clear();
    OnDisableVec.clear();

    // 이벤트 호출
    for (auto& component : validComponents)
    {
        if (_isPlay && component->_initFlags.IsStart())
        {
            component->OnDisable();
        }
    }
    validComponents.clear();
}

void ESceneManager::ObjectsDestroy()
{
    //컴포넌트 삭제
    auto& [destroyComponentSet, destroyComponentQueue] = _destroyComponentsQueue;
    // OnDestroy 호출 도중 원본 큐 변형 방지를 위한 지연삭제
    _destroyComponentsTemp = destroyComponentQueue;
    destroyComponentQueue.clear();
    for (auto& destroyComponent : _destroyComponentsTemp)
    {
        // OnDestroy 대상 호출
        if (_isPlay)
        {
            destroyComponent->OnDestroy();
        }

        // 해당 컴포넌트를 오브젝트 배열에서 삭제.
        std::vector<std::shared_ptr<Component>>& components = destroyComponent->_gameObject->_components;
        std::erase_if(components, [destroyComponent](std::shared_ptr<Component>& component) 
        {
            return destroyComponent == component.get();
        });
    }
    destroyComponentSet.clear();
    for (auto& component : destroyComponentQueue)
    {
        destroyComponentSet.insert(component);
    }
    
    //오브젝트 삭제
    auto& [destroyObjectSet, destroyObjectQueue] = _destroyObjectsQueue;
    // OnDestroy 호출 도중 원본 큐 변형 방지를 위한 복사 후 삭제
    _destroyObjectTemp = destroyObjectQueue;
    destroyObjectQueue.clear();
    for (auto& destroyObject : _destroyObjectTemp)
    {
        if (_isPlay)
        {
            for (auto& component : destroyObject->_components)
            {
                component->OnDestroy();
            }
        }

        //오브젝트 삭제
        int instanceID = destroyObject->GetInstanceID();
        if (STR_NULL != destroyObject->_ownerScene)
        {
            if (static_cast<size_t>(instanceID) < _runtimeObjects.size() && _runtimeObjects[instanceID])
            {
                std::shared_ptr<GameObject>& pObject = _runtimeObjects[instanceID];
                SetObjectOwnerScene(pObject.get(), STR_NULL);
                EraseGameObjectMap(pObject);
                pObject.reset();
            }
            else
            {
                std::erase_if(_addGameObjectsQueue, [destroyObject, this](std::shared_ptr<GameObject>& object) 
                {
                    bool erase = object.get() == destroyObject;
                    if (erase)
                    {
                        SetObjectOwnerScene(object.get(), STR_NULL);
                        EraseGameObjectMap(object);
                    }
                    return erase;
                });

            }
        }
    }
    destroyObjectSet.clear();
    for (auto& object : destroyObjectQueue)
    {
        destroyObjectSet.insert(object);
    }

    //배열 정리
    while (_runtimeObjects.empty() == false && _runtimeObjects.back() == nullptr)
    {
        _runtimeObjects.pop_back();
    }

    //큐 초기화
    if (_destroyComponentsTemp.empty() || _destroyObjectTemp.empty())
    {
        UmComponentFactory.CleanupExpiredComponents();
        _destroyComponentsTemp.clear();
        _destroyObjectTemp.clear();
    }
}

void ESceneManager::ObjectsAddRuntime()
{
    //오브젝트 추가
    static std::unordered_set<Transform*> updateMatrixSet;
    updateMatrixSet.clear();
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

        Transform* root = (nullptr != gameObject->_transform._root) ? gameObject->_transform._root : &gameObject->_transform;
        auto [iter, insertResult] = updateMatrixSet.insert(gameObject->_transform.Root);
        if (insertResult)
        {
            root->UpdateMatrix();     
        }
        GameObject::Engine::UpdateActiveInHierarchy(gameObject.get());     
    }
    _addGameObjectsQueue.clear();

    //임시 큐
    static thread_local std::vector<Component*> addQueue;
    addQueue.reserve(_addComponentsQueue.size());
    for (auto& [owner, component] : _addComponentsQueue)
    {   
        if (owner.expired() == false)
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
            addQueue.push_back(component.get());
        }
    }
    _addComponentsQueue.clear();

    //안전하게 원본 배열에서 복사 후 이벤트 호출
    for (auto& component : addQueue)
    {
        component->UpdateEnableInHierarchy();
        component->Added();
    }
    addQueue.clear();
}

void ESceneManager::ResourceManagerUpdate() 
{
    SceneResourceManager::Engine::Update(ResourceManager);
    if (_waitResourceLoad)
    {
        _checkResourceLoad = _waitResourceLoad;
        _waitResourceLoad  = false;
    }       
}

bool ESceneManager::IsRuntimeActive(std::shared_ptr<GameObject>& obj)
{
    return obj.get() != nullptr && obj->_activeInHierarchy && obj->IsValid();
}

void ESceneManager::NotInitDestroyComponentEraseToWaitVec(Component* destroyComponent)
{
    if (destroyComponent->_initFlags.IsAwake() == false)
    {
        size_t result = std::erase_if(_waitAwakeVec, [destroyComponent](std::shared_ptr<Component>& component) 
        {
            return component.get() == destroyComponent;
        });

        if (0 == result)
        {
            if (false == _addComponentsQueue.empty())
            {
                // 추가 대기중인 컴포넌트라면 같이 삭제
                std::erase_if(_addComponentsQueue,
                [destroyComponent](const std::pair<std::weak_ptr<GameObject>, std::shared_ptr<Component>>& pair) 
                {
                    auto& [obj, component] = pair;
                    return component.get() == destroyComponent;
                });
            }
        }         
    }

    if (destroyComponent->_initFlags.IsStart() == false)
    {
        if (false == _waitStartVec.empty())
        {
            std::erase_if(_waitStartVec, [destroyComponent](std::shared_ptr<Component>& component) 
            {
                return component.get() == destroyComponent;
            });
        }
    }
}

bool ESceneManager::InsertGameObjectMap(std::shared_ptr<GameObject>& insertObject) 
{
    auto [iter, result] = _runtimeObjectsUnorderedMap[insertObject->ReflectFields->_name].insert(insertObject);
    if (result == false)
    {
        assert(!"이미 추가한 게임 오브젝트 입니다.");
    }
    return result;
}

void ESceneManager::EraseGameObjectMap(std::shared_ptr<GameObject>& eraseObject)
{
    auto findIter = _runtimeObjectsUnorderedMap.find(eraseObject->ReflectFields->_name);
    if (findIter == _runtimeObjectsUnorderedMap.end())
    {
        assert(!"유효하지 않는 오브젝트 이름입니다.");
    }
    findIter->second.erase(eraseObject);
}

void ESceneManager::AddDestroyComponentQueue(Component* component) 
{
    if (component && component->gameObject->IsValid())
    {
        auto& [set, vec]    = engineCore->SceneManager._destroyComponentsQueue;
        auto [iter, result] = set.insert(component);
        if (result)
        {
            vec.push_back(component);
            NotInitDestroyComponentEraseToWaitVec(component);
        }
    }
}

void ESceneManager::SetObjectOwnerScene(GameObject* object, std::string_view sceneName) 
{
    object->_ownerScene = sceneName;
}

void ESceneManager::SetRendererSkyBox(Scene* scene) 
{
    // ENV 로드
    if (STR_NULL != scene->_skyBox)
    {
        bool loadSkyBox = false;
        if (STR_NULL != _prevScene)
        {
            File::Guid prevGuid = UmFileSystem.GetGuidFromPath(_prevScene);
            if (false == prevGuid.IsNull())
            {
                Scene& prevScene = _scenesMap[prevGuid];
                if (prevScene._skyBox != scene->_skyBox)
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
                UmGraphics.SetEnvironmentSkyBox("Game", path.c_str());

                if constexpr (IS_EDITOR)
                {
                    UmGraphics.SetEnvironmentSkyBox("Editor", path.c_str());
                }
            }
        }
    }
    else
    {
        UmGraphics.ResetEnvironmentSkyBox("Game");
        if constexpr (IS_EDITOR)
        {
            UmGraphics.ResetEnvironmentSkyBox("Editor");
        }
    }

    // IBL 로드
    if (STR_NULL != scene->_skyIBL)
    {
        bool loadSkyBox = false;
        if (STR_NULL != _prevScene)
        {
            File::Guid prevGuid = UmFileSystem.GetGuidFromPath(_prevScene);
            if (false == prevGuid.IsNull())
            {
                Scene& prevScene = _scenesMap[prevGuid];
                if (prevScene._skyIBL != scene->_skyIBL)
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
            File::Path path = scene->_skyIBL.ToPath();
            if (false == path.IsNull())
            {
                UmGraphics.SetIBLSkyBox("Game", path.c_str());

                if constexpr (IS_EDITOR)
                {
                    UmGraphics.SetIBLSkyBox("Editor", path.c_str());
                }
            }
        }
    }
    else
    {
        UmGraphics.ResetIBLSkyBox("Game");
        if constexpr (IS_EDITOR)
        {
            UmGraphics.ResetIBLSkyBox("Editor");
        }
    }
}

void ESceneManager::AddDestroyObjectQueue(GameObject* gameObject) 
{
    if (gameObject && gameObject->IsValid())
    {
        auto& [set, vec] = engineCore->SceneManager._destroyObjectsQueue;
        auto& componentDestroySet = engineCore->SceneManager._destroyComponentsQueue.first;
        Transform::ForeachDFS(gameObject->_transform, [this, &set, &vec, &componentDestroySet](Transform* pTransform) {
            auto [iter, result] = set.insert(&pTransform->gameObject);
            if (result)
            {
                vec.push_back(&pTransform->gameObject);
                for (auto& component : pTransform->_gameObject._components)
                {
                    componentDestroySet.insert(component.get());
                    NotInitDestroyComponentEraseToWaitVec(component.get());
                }
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
    if (false == targetScene._skyIBL.ToPath().IsNull())
    {
        sceneNode["SkyIBL"] = targetScene._skyIBL.string();
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
    UmGraphics.SetEnvironmentSkyBox("Game", path.c_str());

    if constexpr (IS_EDITOR)
    {
        UmGraphics.SetEnvironmentSkyBox("Editor", path.c_str());
    }

    mainScene->IsDirty = true;

    return true;
}

bool ESceneManager::SetSkyIBL(const File::Path& path)
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

    Engine::SetSceneIBLGuid(*mainScene, guid);
    UmGraphics.SetIBLSkyBox("Game", path.c_str());

    if constexpr (IS_EDITOR)
    {
        UmGraphics.SetIBLSkyBox("Editor", path.c_str());
    }

    mainScene->IsDirty = true;

    return true;
}

const std::vector<std::weak_ptr<MeshComponent>>& ESceneManager::GetMeshComponents()
{
    ClearExpiredMeshComponents();
    return _runtimeMeshComponents;
}

void ESceneManager::ClearExpiredMeshComponents() 
{
    std::erase_if(_runtimeMeshComponents, [](const std::weak_ptr<MeshComponent>& weakMesh) 
    { 
        return weakMesh.expired();
    });
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
    if (writePath.extension() != SCENE_EXTENSION)
    {
        writePath += SCENE_EXTENSION;
    }
   
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
            if (sceneNode["SkyIBL"])
            {
                scene._skyIBL = sceneNode["SkyIBL"].as<std::string>();
            }
            else
            {
                // SkyIBL 노드가 없던 구 버전 씬 파일은 skyBox를 IBL로
                scene._skyIBL = scene._skyBox;
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
            if (node["SkyIBL"])
            {
                scene._skyIBL = node["SkyIBL"].as<std::string>();
            }
            else
            {
                // SkyIBL 노드가 없던 구 버전 씬 파일은 skyBox를 IBL로
                scene._skyIBL = scene._skyBox;
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
    std::list<std::tuple<std::weak_ptr<Component>, File::Path, std::function<void()>>> tempResource;
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
                        auto SafeCallbackFunc = [weakPtr, path, func]() 
                        {
                            if (std::shared_ptr<Component> component = weakPtr.lock())
                            {
                                if (true == std::filesystem::exists(path))
                                {
                                    if (component->_gameObject->IsValid())
                                    {
                                        func();
                                    }
                                }
                            }
                        };

                        path = std::filesystem::absolute(path);
                        path = path.generic_string();
                        auto findIter = resource.RenderResource.find(path);
                        if (findIter == resource.RenderResource.end())
                        {
                            auto newResource = UmResourceManager->LoadResource<T>(path.string(), SafeCallbackFunc);
                            resource.RenderResource[path] = newResource;
                        }
                        else
                        {
                            std::shared_ptr<T> resource = UmResourceManager->LoadResource<T>(path.string(), nullptr);
                            if (resource->IsValid())
                            {
                                func();
                            }
                            else
                            {
                                tempResource.push_back(curr);
                                continue;
                            }
                        }
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

    for (auto& tuple : tempResource)
    {
        resource.ResourceLoadQueue.push(tuple);
    }
}

void ESceneManager::SceneResourceManager::Engine::Update(SceneResourceManager& manager)
{
    ESceneManager& sceneManager = UmSceneManager;
    if (true == sceneManager._addComponentsQueue.empty())
    {
        manager.UpdateRenderResource(manager._models);
        manager.UpdateRenderResource(manager._textures);
        manager.UpdateRenderResource(manager._fonts);
        manager.UpdateRenderResource(manager._sdfFonts);
    }
}

template <typename T>
bool CheckReadyResource(T& resource)
{
    for (auto& [path, resource] : resource.RenderResource)
    {
        if (false == resource->IsValid())
            return false;
    }
    return true;
}

template <>
bool CheckReadyResource(ESceneManager::SceneResourceManager::RenderResource<Model>& resource)
{
    for (auto& [path, resource] : resource.RenderResource)
    {
        if (false == resource->IsValid())
            return false;

        //TODO: 씬 리소스 로드 대기해야 하는데 안대요
        /*
        for (auto& textures : resource->GetTextures())
        {
            for (auto& texture : textures)
            {
                if (nullptr == texture)
                    return false;

                if (false == texture->IsValid())
                    return false;                   
            }
        }
        */
    }
    return true;
}

bool ESceneManager::SceneResourceManager::CheckAllResourceLoad()
{
    if (false == CheckReadyResource(_models))
    {
        return false;
    }
    if (false == CheckReadyResource(_textures))
    {
        return false;
    }
    if (false == CheckReadyResource(_fonts))
    {
        return false;
    }
    if (false == CheckReadyResource(_sdfFonts))
    {
        return false;
    }
    return true;
}

void ESceneManager::SceneResourceManager::ClearResource() 
{
    _models.RenderResource.clear();

    _textures.RenderResource.clear();

    _fonts.RenderResource.clear();

    _sdfFonts.RenderResource.clear();
}

void ESceneManager::SceneResourceManager::Engine::CleanUp(SceneResourceManager& manager) 
{
    manager._models.ResourceLoadQueue.clear();
    manager._models.RenderResource.clear();

    manager._textures.ResourceLoadQueue.clear();
    manager._textures.RenderResource.clear();

    manager._fonts.ResourceLoadQueue.clear();
    manager._fonts.RenderResource.clear();

    manager._sdfFonts.ResourceLoadQueue.clear();
    manager._sdfFonts.RenderResource.clear();
}

void ESceneManager::SceneResourceManager::RequestModelResource(const Component* component, const File::Guid& guid,
                                                               const std::function<void()>& func)
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
                                                               const std::function<void()>& func)
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

void ESceneManager::SceneResourceManager::RequestTextureResource(const Component* component, const File::Guid& guid, const std::function<void()>& func)
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
                                                                 const std::function<void()>& func)
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
                                                              const std::function<void()>& func)
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
                                                              const std::function<void()>& func)
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

void ESceneManager::SceneResourceManager::RequestSDFFontResource(const Component* component, const File::Guid& guid,
                                                                 const std::function<void()>& func)
{
    if (component->gameObject->IsValid())
    {
        File::Path path = UmFileSystem.GetPathFromGuid(guid);
        if (false == path.IsNull())
        {
            RequestSDFFontResource(component, path, func);
        }
        else
        {
            std::string_view   componentName = component->ClassName();
            const std::string& objectName    = component->gameObject->Name;
            std::string msg = std::format("{}{}{} {}", guid.string(), (const char*)u8"는 존재하지 않는 리소스입니다. ", objectName, componentName);
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        }
    }
}

void ESceneManager::SceneResourceManager::RequestSDFFontResource(const Component* component, const File::Path& path,
                                                                 const std::function<void()>& func)
{
    if (component->gameObject->IsValid())
    {
        if (true == std::filesystem::exists(path))
        {
            auto tuple = std::make_tuple(component->GetWeakPtr(), path, func);
            _sdfFonts.ResourceLoadQueue.push(tuple);
        }
        else
        {
            std::string_view   componentName = component->ClassName();
            const std::string& objectName    = component->gameObject->Name;
            std::string msg = std::format("{}{}{} {}", path.string(), (const char*)u8"는 존재하지 않는 리소스입니다. ", objectName, componentName);
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        }
    }
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
            _actionTracker.fill(Action::IDLE);
            if (const auto& queue = _inputController.GetButtonQueue(); false == queue.empty())
            {
                for (const auto& state : queue)
                {
                    UpdateTracker(state);
                    CallInputReceiver(state.Button);
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

void ESceneManager::InputSystem::RegisterInputReceiver(InputReceiver& receiver, int buttonIndex, int actionIndex, std::function<void(const Input::Controller& controller)> func)
{
    constexpr int maxButtonCount = static_cast<int>(ControllerButton::UNKNOWN);
    constexpr int maxActionCount = static_cast<int>(Action::UNKNOWN);

    if (buttonIndex < maxButtonCount && actionIndex < maxActionCount)
    {
        auto& receiverTarget = _receivers[buttonIndex][actionIndex];
        if (nullptr == receiver._isDestroy)
        {
            // 플래그 bool 값을 동적 할당
            receiverTarget.emplace_back(std::make_shared<bool>(false), func);
            receiver._isDestroy = receiverTarget.back().first;
        }
        else
        {
            // 이미 등록된 리시버는 bool 값을 공유.
            receiverTarget.emplace_back(receiver._isDestroy, func);
        }
    } 
}

bool ESceneManager::InputSystem::PushReceiverToInputStack(InputReceiver& receiver)
{
    if (receiver._isDestroy && false == *receiver._isDestroy)
    {
        if (false == receiver._isPushStack)
        {
            _layerStack.emplace_back(receiver._isDestroy.get(), receiver._isDestroy);
            receiver._isPushStack = true;
            return true;
        }
    }  
    return false;
}

bool ESceneManager::InputSystem::PopReceiverToInputStack(InputReceiver& receiver)
{
    if (receiver._isDestroy)
    {
        bool result = false;
        while (false == _layerStack.empty())
        {
            auto& [topReceiver, isDestroy] = _layerStack.back();
            if (receiver._isPushStack && receiver._isDestroy.get() == topReceiver)
            {
                _layerStack.pop_back();
                receiver._isPushStack = false;
                result = true;
            }
            else 
            {               
                if (auto destroyFlag = isDestroy.lock())
                {
                    if (false == *destroyFlag)
                    {
                        //유효한 InputLayer면 반환
                        return result;  
                    }
                }
                _layerStack.pop_back(); //유효하지 않는 레이어는 제거 
            }        
        }      
    }
    return false;
}

void ESceneManager::InputSystem::CleanupInputReceivers() 
{
    for (auto& actions : _receivers)
    {
        for (auto& inputReceivers : actions)
        {
            inputReceivers.clear();
        }
    }
    _layerStack.clear();
}

void ESceneManager::InputSystem::Vibrate(const Input::ControllerTypes::Vibration vibration)
{
    _inputController.Vibrate(vibration);
}

void ESceneManager::InputSystem::StopVibration() 
{
    _inputController.Vibrate(0, 0);
}

struct GetIndex
{
    size_t operator()(const Input::ControllerTypes::Button button) const
    {
        return std::countr_zero(static_cast<size_t>(button));
    }

    size_t operator()(const ESceneManager::InputSystem::Action action) const { return static_cast<size_t>(action); }
};

void ESceneManager::InputSystem::UpdateTracker(const Input::Controller::ButtonState& buttonState)
{
    const size_t buttonIndex = GetIndex()(buttonState.Button);

    Action& action = _actionTracker[buttonIndex];

    switch (buttonState.Flag)
    {
    case Input::ControllerTypes::STATE_DOWN:
        action = Action::PRESSED;
        break;
    case Input::ControllerTypes::STATE_UP:
        action = Action::RELEASED;
        break;
    case Input::ControllerTypes::STATE_REPEAT:
        action = Action::HELD;
        break;
    }
}

void ESceneManager::InputSystem::CallInputReceiver(const Input::Controller::Button button)
{
    constexpr GetIndex getIndex;
    const size_t       buttonIndex = getIndex(button);
    const Action action = _actionTracker[buttonIndex];
    const size_t actionIndex = getIndex(action);

    auto& receivers   = _receivers[buttonIndex][actionIndex];
    bool  activeErase = false;
    for (auto& [isDestroy, event] : receivers)
    {
        if (nullptr == isDestroy || true == *isDestroy)
        {
            activeErase = true;
        }
        else
        {
            if (_layerStack.empty())
            {
                event(_inputController);
            }
            else
            {
                if (auto& [destroyFlag, weak] = _layerStack.back(); destroyFlag == isDestroy.get())
                {
                    event(_inputController);
                }
            }
        }
    }

    if (true == activeErase)
    {
        std::erase_if(receivers, [](auto& pair) {
            auto& [destroy, event] = pair;
            return nullptr == destroy || *destroy;
        });

        while (false == _layerStack.empty())
        {
            if (auto& [topReceiver, isDestroy] = _layerStack.back(); true == isDestroy.expired())
            {
                _layerStack.pop_back();
            }
            else
            {
                break;
            }
        }
    }
}