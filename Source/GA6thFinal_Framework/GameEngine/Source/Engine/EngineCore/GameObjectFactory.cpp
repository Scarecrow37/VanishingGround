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

void EGameObjectFactory::Engine::Finalize()
{
    EGameObjectFactory& factory = UmGameObjectFactory;
    factory._prefabObjectMap.clear();
    factory._prefabInstanceList.clear();
    factory._newGameObjectFuncMap.clear();
    factory._newGameObjectKeyVec.clear();
    factory._prefabObjectMap.clear();
    factory._prefabGuidQueue.clear();
    factory._prefabInstanceList.clear();
    factory._prefabInstanceOverride.clear();
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
    static EditorHierarchyTool* editorHierarchyTool = EditorHierarchyTool::GetTool();
    if (editorHierarchyTool)
    {
        auto findIter = _prefabInstanceList.find(guid);
        if (findIter != _prefabInstanceList.end())
        {
            auto& [guid, list] = *findIter;
            std::erase_if(list, [](auto& waek) 
            { 
                std::shared_ptr<GameObject> instance = waek.lock();
                return nullptr == instance || false == instance->IsValid();
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
                    YAML::Node myYaml = SerializeToYaml(gameObject.get(), true);
                    auto prefabObjects = MakeObjectsGraphToYaml(&yaml, true, &myYaml);
                    if (false == prefabObjects.empty())
                    {
                        std::vector<std::pair<GameObject*, GameObject*>> swapObjects;
                        swapObjects.reserve(prefabObjects.size());
                        int i = 0;
                        Transform::ForeachBFS(gameObject->_transform, [&](Transform* curr) 
                        {
                            if (i < prefabObjects.size())
                            {
                                swapObjects.emplace_back(&curr->gameObject, prefabObjects[i].get());
                            }
                            else
                            {
                                GameObject::Destroy(curr->gameObject);
                            }
                            i++;
                        });

                        if (false == swapObjects.empty())
                        {
                            // 최상위 오브젝트 Transform 설정
                            auto& [frontOrigin, frontPrefab] = swapObjects.front();
                            frontPrefab->_ownerScene         = frontOrigin->_ownerScene;
                            Transform* frontParent = frontOrigin->transform->Parent;
	    					if (nullptr != frontParent)
	    					{
                                for (int childIndex = 0; childIndex < (int)frontParent->_childsList.size(); ++childIndex)
                                {
                                    if (&frontOrigin->_transform == frontParent->_childsList[childIndex])
                                    {
                                        Transform* prefabTransform = &frontPrefab->transform;
                                        frontParent->_childsList[childIndex] = prefabTransform;
                                        prefabTransform->_parent             = frontParent;
                                        prefabTransform->_root               = frontParent->_root;
                                        if (nullptr == prefabTransform->_root)
                                        {
                                            prefabTransform->_root = prefabTransform->_parent;
                                        }
                                        frontOrigin->_transform.EraseParent(false);
                                        break;
                                    }
                                }
	    					}
	    					else
	    					{
                                frontPrefab->transform->SetParentEx(frontParent, false, false);
	    					}
                         
                            // Swap 이루어진 오브젝트들
                            std::vector<std::shared_ptr<GameObject>> originInstances;
                            originInstances.reserve(swapObjects.size());
                            for (auto& [originObject, prefabObject] : swapObjects)
                            {              
                                std::shared_ptr<GameObject> origin = ESceneManager::Engine::SwapPrefabInstance(originObject, prefabObject);
                                if (origin)
                                {
                                    originInstances.push_back(std::move(origin));
                                }
                            }

                            // 소멸자 지연 호출
                            for (auto& origin : originInstances)
                            {
                                GameObject::Engine::ResetActiveInHierarchy(origin.get());
                            }
                            originInstances.clear();
                        }
                       
                        //프리팹과 구조가 다른 없는 오브젝트 추가
                        if (i < prefabObjects.size())
                        {
                            std::string_view ownerScene = prefabObjects[0]->_ownerScene;
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
}

void EGameObjectFactory::ErasePrefabItem(const File::Guid& guid) 
{
    _prefabObjectMap.erase(guid);
    _prefabInstanceList.erase(guid);
}

void EGameObjectFactory::OnFileRegistered(const File::Path& path)
{
    File::Guid guid = path.ToGuid();
    auto [yamlData, result] = YAMLHelper::SafeLoadFile(path);
    if (result)
    {
        try
        {
            _prefabObjectMap[guid] = MakeObjectsGraphToYaml(&yamlData, true);
            WritePrefabGuid(path, yamlData);
            ApplyPrefabInstanceChanges(guid, yamlData);
        }
        catch (const std::exception& ex)
        {
             std::string msg = std::format("{}{} {}", (const char*)u8"올바르지 않은 UmPrefab 파일입니다. ", path.string(), ex.what());
             UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
             ErasePrefabItem(guid);
        }
    }
    else
    {
        std::string msg = std::format("{}{} {}", (const char*)u8"올바르지 않은 UmPrefab 파일입니다. ", path.string(), result.What());
        UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
    }
}

void EGameObjectFactory::OnFileUnregistered(const File::Path& path) 
{
    File::Guid guid = path.ToGuid();
    ErasePrefabItem(guid);
}

void EGameObjectFactory::OnFileModified(const File::Path& path)
{
    File::Guid guid = path.ToGuid();
    auto [yamlData, result] = YAMLHelper::SafeLoadFile(path);
    if (result)
    {
        try
        {
            _prefabObjectMap[guid] = MakeObjectsGraphToYaml(&yamlData, true);
            WritePrefabGuid(path, yamlData);
            ApplyPrefabInstanceChanges(guid, yamlData);
        }
        catch (const std::exception& ex)
        {
            std::string msg = std::format("{}{} {}", (const char*)u8"올바르지 않은 UmPrefab 파일입니다. ",
                                          path.string(), ex.what());
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
            ErasePrefabItem(guid);
        }
    }
    else
    {
        std::string msg = std::format("{}{} {}",  (const char*)u8"올바르지 않은 UmPrefab 파일입니다. ", path.string(), result.What());
        UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        ErasePrefabItem(guid);
    }
}

void EGameObjectFactory::OnFileRemoved(const File::Path& path) 
{
    File::Guid guid = path.ToGuid();
    ErasePrefabItem(guid);
}

void EGameObjectFactory::OnFileRenamed(const File::Path& oldPath, const File::Path& newPath) 
{

}

void EGameObjectFactory::OnFileMoved(const File::Path& oldPath, const File::Path& newPath) 
{

}

void EGameObjectFactory::OnRequestedSave() 
{

}

void EGameObjectFactory::OnRequestedLoad() 
{
    _prefabInstanceList.clear();
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

YAML::Node EGameObjectFactory::SerializeToYaml(GameObject* gameObject, bool onlyValidObject)
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
    Transform::ForeachExBFS(
    gameObject->_transform, 
    onlyValidObject,
    [&](Transform* curr) 
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

    std::map<int, std::shared_ptr<GameObject>>					   transformParentLevelMap; // 오브젝트로 인덱스를 기록하는 맵
    std::map<GameObject*, std::queue<std::shared_ptr<GameObject>>> parentPrefabObjectsMap;  // 부모 프리팹인 오브젝트들의 맵

    bool isPrefabInstance = false;
    int nodeIndex = 0;
    for (auto currIter = nodes.begin(); currIter != nodes.end(); ++currIter)
    {
        // 오브젝트 초기화
        std::shared_ptr<GameObject> currObject;
        const YAML::Node& currNode = *currIter;
        std::string Type = currNode["Type"].as<std::string>();
        File::Guid prefabGuid = STR_NULL;
        if (currNode["Prefab"])
        {
            prefabGuid = currNode["Prefab"].as<std::string>();         
        }
        bool isPrefabObject = prefabGuid != STR_NULL;

        YAML::Node transformNode = currNode["Transform"].as<YAML::Node>();
        int transformIndex = transformNode["TransformIndex"].as<int>();
        int parentIndex = -1; 
        if (transformNode["ParentIndex"])
        {
            parentIndex = transformNode["ParentIndex"].as<int>();	
        }

        //오브젝트 생성
        bool isValidNode = true;
        bool isEmptyObject = true;
		if (0 < parentIndex)
		{
            auto findParentIter = transformParentLevelMap.find(parentIndex);
            if (findParentIter != transformParentLevelMap.end())
            {
                if (false == parentPrefabObjectsMap.empty())
                {
                    auto&      parent       = findParentIter->second;
                    Transform* findPrefabTr = &parent->_transform;
                    while (nullptr != findPrefabTr)
                    {
                        if (findPrefabTr->_gameObject._prefabGuid != STR_NULL)
                        {
                            break;
                        }
                        findPrefabTr = findPrefabTr->_parent;
                    }

                    if (findPrefabTr)
                    {              
                        auto iterPrefabFindIter = parentPrefabObjectsMap.find(&findPrefabTr->_gameObject);
                        if (iterPrefabFindIter != parentPrefabObjectsMap.end())
                        {
                            auto& queue = iterPrefabFindIter->second;
                            if (false == queue.empty())
                            {
                                currObject = queue.front();
                                queue.pop();
                                isEmptyObject = false;
                            }
                            else
                            {
                                parentPrefabObjectsMap.erase(iterPrefabFindIter);
                            }
                        }                                              
                    }        
                }          
            }
			else
			{
                isValidNode = false;
			}
		}
             
        if (isValidNode)
        {
            bool callSetParnet = true;
            if (nullptr == currObject) 
            {
                if (isPrefabObject && 0 < nodeIndex)
                {
                    auto prefabIter = _prefabObjectMap.find(prefabGuid);
                    if (prefabIter != _prefabObjectMap.end())
                    {
                        YAML::Node yamlData = SerializeToYaml(prefabIter->second[0].get());
                        auto       objects  = MakeObjectsGraphToYaml(&yamlData, useResource);
                        currObject          = objects.front();
                        isEmptyObject       = false;

                        // 해당 부모에 오브젝트를 순차적으로 가져옴.
                        auto& queue = parentPrefabObjectsMap[currObject.get()];
                        for (int i = 1; i < objects.size(); ++i)
                        {
                            auto& obj = objects[i];
                            queue.push(obj);                           
                        }
                    }
                }

                if (nullptr == currObject)
                {
                    currObject = MakeGameObject(Type);
                    if (useResource == false)
                    {
                        ResetGameObject(currObject.get(), "null");
                    }
                    ParsingYamlToGameObject(currObject.get(), currNode);
                }
            }
            else
            {
                callSetParnet = false; // 이미 프리팹에서 부모 설정 완료했기 때문에 호출할 필요 없음
            }

            // 프리팹 추적
            if (isPrefabObject)
            {
                if (useResource == false)
                {
                    std::vector<std::weak_ptr<GameObject>>& instanceList = _prefabInstanceList[prefabGuid];
                    instanceList.emplace_back(currObject);
                    if (nullptr != pSceneObjectNode)
                    {
                        if (sceneNodes != pSceneObjectNode->end())
                        {
                            const YAML::Node& currSceneNodes = *sceneNodes;
                            ParsingYamlToGameObject(currObject.get(), currSceneNodes);
                        }
                        else
                        {
                            pSceneObjectNode = nullptr;
                        }
                    }
                }
                currObject->_prefabGuid = prefabGuid;
                isPrefabInstance = true;
            }

            // Transform 역직렬화
            transformParentLevelMap[transformIndex] = currObject;
            if (parentIndex != transformIndex)
            {
                if (callSetParnet)
                {
                    if ((isPrefabObject && 0 <= parentIndex) || (isEmptyObject && 0 <= parentIndex))
                    {
                        Transform* pParent = &transformParentLevelMap[parentIndex]->_transform;
                        currObject->_transform.SetParentEx(pParent, false, false);
                    }
                }	
            }

            // 컴포넌트들 역직렬화
            if (currNode["Components"])
            {
                YAML::Node           componentNodes = currNode["Components"];
                YAML::const_iterator sceneComponentNodeIter;
                if (pSceneObjectNode)
                {
                    if (sceneNodes != pSceneObjectNode->end())
                    {
                        const YAML::Node& currSceneNodes = *sceneNodes;
                        sceneComponentNodeIter           = currSceneNodes["Components"].begin();
                    }
                    else
                    {
                        pSceneObjectNode = nullptr;
                    }
                }
                
                int componentIndex = 0;
                for (auto componentNode : componentNodes)
                {
                    Component*  component         = nullptr;
                    YAML::Node& currComponentNode = componentNode;

                    if (isEmptyObject)
                    {
                        if (useResource == false)
                        {
                            component = UmComponentFactory.AddComponentToYamlLifeCycle(currObject.get(), &currComponentNode);
                        }
                        else
                        {
                            component = UmComponentFactory.AddComponentToYamlNow(currObject.get(), &currComponentNode);
                        }
                    }
                    else
                    {
                        component = currObject->GetComponentAtIndex<Component>(componentIndex);
                    }

                    if (component && pSceneObjectNode)
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
                    ++componentIndex;
                }
            }
            makeList.push_back(currObject);
		}
           
        if (nullptr != pSceneObjectNode)
        {
            if (sceneNodes != pSceneObjectNode->end())
            {
                ++sceneNodes;
            }
			else
			{
                pSceneObjectNode = nullptr;
			}
        }
        ++nodeIndex;
    }

    if (false == makeList.empty())
    {            
        //원본 프리팹과 달라서 변경해야할 오브젝트 대상
        std::unordered_set<GameObject*> eraseTargetMap;
        eraseTargetMap.reserve(makeList.size());
        std::vector<std::shared_ptr<GameObject>> pushTargetPrefabObjects;
        pushTargetPrefabObjects.reserve(makeList.size());

        // 게임 오브젝트의 _activeInHierarchy 계산
        for (size_t i = 0; i < makeList.size(); ++i)
        {
            bool resetHierarchy = true;
            auto& object = makeList[i];
            if (false == useResource)
            {
                const File::Guid& objectPrefabGuid = object->_prefabGuid;
                if (objectPrefabGuid != STR_NULL)
                {
                    auto prefabIter = _prefabObjectMap.find(objectPrefabGuid);
                    if (prefabIter != _prefabObjectMap.end())
                    {
                        // 원본 프리팹과 계층구조 일치하도록 변경.
                        auto&  originPrefabRoot      = prefabIter->second.front();
                        size_t originDescendantCount = 0;
                        Transform::ForeachExDFS(originPrefabRoot->_transform, false, [&originDescendantCount](Transform* curr) 
                        { 
                            ++originDescendantCount; 
                        });

                        size_t currObjDescedantCount = 0;
                        Transform::ForeachExDFS(object->_transform, false, [&](Transform* curr) 
                        {
                            ++currObjDescedantCount;
                            if (originDescendantCount < currObjDescedantCount)
                            {
                                eraseTargetMap.insert(&curr->_gameObject);
                            }
                        });

                        if (originDescendantCount < currObjDescedantCount)
                        {
                            resetHierarchy = false;
                        }
                    }
                }
                
                //프리팹에만 있어서 추가 해야하는 오브젝트들
                auto iterPrefabFindIter = parentPrefabObjectsMap.find(object.get());
                if (iterPrefabFindIter != parentPrefabObjectsMap.end())
                {
                    auto& queue = iterPrefabFindIter->second;
                    while (false == queue.empty())
                    {
                        pushTargetPrefabObjects.push_back(std::move(queue.front()));
                        queue.pop();
                    }
                    parentPrefabObjectsMap.erase(iterPrefabFindIter);                   
                }                                              
            }

            if (resetHierarchy)
            {
                GameObject::Engine::ResetActiveInHierarchy(object.get());
            }
        }

        // 원본 프리팹과 달라서 지워야할 오브젝트 제거
        if (false == eraseTargetMap.empty())
        {
            std::erase_if(makeList, [&eraseTargetMap](auto& gameObject) 
            { 
                return eraseTargetMap.find(gameObject.get()) != eraseTargetMap.end();
            });
        }

        //원본 프리팹과 달라서 추가해야할 오브젝트 추가
        if (false == pushTargetPrefabObjects.empty())
        {
            for (auto& pushObject : pushTargetPrefabObjects)
            {
                makeList.push_back(std::move(pushObject));
            }

            //BFS 기준으로 재정렬 해야함
            pushTargetPrefabObjects.clear();
            Transform::ForeachExBFS(makeList.front()->_transform, false, [&pushTargetPrefabObjects](Transform* curr) 
            {
                pushTargetPrefabObjects.push_back(curr->_gameObject.GetWeakPtr().lock());
            });
            makeList = std::move(pushTargetPrefabObjects);
        }
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

    auto prefabIter = _prefabObjectMap.find(guid);
    if (prefabIter == _prefabObjectMap.end())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"존재하지 않은 프리팹 입니다.."_c_str);
        return nullptr;
    }
    YAML::Node yamlData = SerializeToYaml(prefabIter->second[0].get());
    auto pObject = DeserializeToYaml(&yamlData, sceneNode);
    PackPrefab(pObject.get(), guid);
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
        newObject = UmGameObjectFactory.DeserializeToYaml(&sceneObjectsNode, &sceneObjectsNode);
    }
    return newObject;
}

void EGameObjectFactory::WriteGameObjectFile(Transform* transform, std::string_view outPath)
{
    if (true == UmCore->IsPlay())
    {
        return; //플레이 할때는 불가능
    }

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
    YAML::Node node = UmGameObjectFactory.SerializeToYaml(&transform->gameObject, true);
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
    // UI 내비게이션은 프리팹 금지
    bool isNavi = false;
    Transform::ForeachBFS(targetObject->_transform, [&isNavi](Transform* curr) 
    {
        GameObject& currObject = curr->_gameObject;
        for (auto& component : currObject._components)
        {
            if (Component::TYPE::UI == component->GetType())
            {
                if (UINavigationComponent* navi = dynamic_cast<UINavigationComponent*>(component.get()))
                {
                    isNavi = true;
                }
            }
        }
    });

    if (isNavi)
    {
        UnpackPrefab(targetObject);
        return false;
    }

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
        }
        targetObject->_prefabGuid = STR_NULL;
        return true;
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
    auto findIter = _newGameObjectFuncMap.find(typeid_name.data());
    if (findIter != _newGameObjectFuncMap.end())
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
        ownerObject->ReflectFields->_name       = name;
        ownerObject->ReflectFields->_isStatic   = false;
        ownerObject->ReflectFields->_activeSelf = true;

        // 인스턴스 아이디 부여
        int instanceID           = InstanceID.CreateInstanceID();
        ownerObject->_instanceID = instanceID;
        ownerObject->_creationFrame = UmTime.FrameCount();
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"씬이 로드되지 않았습니다."_c_str);
    }   
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
    return engineCore->GameObjectFactory._newGameObjectKeyVec;
}


