#include "pch.h"
using namespace Global;
using namespace u8_literals;

static constexpr const char* DONT_DESTROY_ON_LOAD_SCENE_NAME = "DontDestroyOnLoad";

bool Scene::RootGameObjectsFilter(GameObject* obj) const
{
    return &obj->GetScene() == this && obj->transform->Parent == nullptr;
}

ESceneManager::ESceneManager() = default;
ESceneManager::~ESceneManager() = default;

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

    auto find = SceneManager._buildScnes.find(DONT_DESTROY_ON_LOAD_SCENE_NAME);
    Scene* pDontDestroyScene = nullptr;
    if (find == SceneManager._buildScnes.end())
    {
        pDontDestroyScene = &SceneManager.CreateScene(DONT_DESTROY_ON_LOAD_SCENE_NAME);
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

std::weak_ptr<GameObject> 
ESceneManager::Engine::GetRuntimeObjectWeakPtr(
    int instanceID)
{
    ESceneManager& SceneManager = engineCore->SceneManager;
    std::weak_ptr<GameObject> weakPtr{}; 
    if (0 <= instanceID && instanceID < SceneManager._runtimeObjects.size())
    {
        if (SceneManager._runtimeObjects[instanceID])
        {
            weakPtr = SceneManager._runtimeObjects[instanceID];
        }
    }
   return weakPtr;
}

Scene& ESceneManager::CreateScene(std::string_view sceneName)
{
    auto find = _buildScnes.find(sceneName.data());
    if(find != _buildScnes.end())
    {
        assert(!"이미 존재하는 씬 입니다.");
        return find->second;
    }

    Scene& newScene = _buildScnes[sceneName.data()];
    newScene.ReflectFields->_filePath = sceneName.data();
    newScene.ReflectFields->_filePath.replace_extension(L".UmScene");
    return newScene;
}

void ESceneManager::LoadScene(std::string_view sceneName, LoadSceneMode mode)
{
    auto find = _buildScnes.find(sceneName.data());
    if (find == _buildScnes.end())
    {
        MessageBox(Global::engineCore->App.GetHwnd(), L"존재하지 않는 씬입니다.",
                   L"씬 로드 실패.", MB_OK);
        return;
    }

    if (mode == LoadSceneMode::SINGLE)
    {
        _addComponentsQueue.clear();
        _addGameObjectsQueue.clear();

        for (auto& [name, scene] : _buildScnes)
        {
            if (name == DONT_DESTROY_ON_LOAD_SCENE_NAME)
                continue;

            scene._isLoaded = false;
            auto objects = scene.GetRootGameObjects();
            for (auto& obj : objects)
            {
                GameObject::Destroy(obj.get());
            }
        }
        _mainScene = sceneName;
    }
    else
    {
        if (find->second._isLoaded)
        {
            engineCore->EngineLogger.Log(
                LogLevel::LEVEL_ERROR,
                u8"이미 로드된 씬은 추가 로드가 불가능합니다."_c_str);
            return;
        }
    }

    auto& [name, scene] = *find;
    scene._isLoaded = true;
    //역직렬화 추가해야함

}

Scene* ESceneManager::GetSceneByName(std::string_view name)
{
    auto find = _buildScnes.find(name.data());
    if (find != _buildScnes.end())
    {
        return &find->second;
    }
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

