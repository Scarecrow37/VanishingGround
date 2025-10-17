#include "pch.h"
#include "EditorSceneCommands.h"

Command::EditorScene::DestroyGameObjectCommand::DestroyGameObjectCommand(GameObject* object) : UmCommand("Destroy Object")
{
    Transform::ForeachBFS(object->transform, [this](Transform* curr) {
        if (curr->gameObject->IsValid())
        {
            _destroyObjects.push_back(curr->gameObject->GetWeakPtr().lock());
        }
    });
    auto& rootObject = _destroyObjects.front();
    _isFocus         = false;
    _active          = rootObject->ActiveSelf;
    _ownerSceneName  = rootObject->GetOwnerSceneName();
}

Command::EditorScene::DestroyGameObjectCommand::~DestroyGameObjectCommand() = default;

bool Command::EditorScene::DestroyGameObjectCommand::Execute()
{
    auto& rootObject               = _destroyObjects.front();
    rootObject->GetScene().IsDirty = true;

    int instanceID         = rootObject->GetInstanceID();
    if (false == UmCore->IsPlay())
    {
        rootObject->ActiveSelf = false;
    }

    UmSceneManager.AddDestroyObjectQueue(rootObject.get());

    if (EditorHierarchyTool::GetFocusObject().lock() == rootObject)
    {
        std::weak_ptr<GameObject> empty;
        EditorHierarchyTool::SetFocusObject(empty);
        _isFocus = true;
    }
    if (EditorInspectorTool::GetFocusObject().lock() == rootObject)
    {
        std::weak_ptr<GameObject> empty;
        EditorInspectorTool::SetFocusObject(empty, true);
        _isFocus = true;
    }

    return true;
}

void Command::EditorScene::DestroyGameObjectCommand::Undo()
{
    auto& rootObject       = _destroyObjects.front();
    rootObject->ActiveSelf = _active;
    for (auto& object : _destroyObjects)
    {
        UmSceneManager.SetObjectOwnerScene(object.get(), _ownerSceneName);
        rootObject->GetScene().IsDirty = true;
        ESceneManager::Engine::AddGameObjectToLifeCycle(object);
    }

    if (_isFocus)
    {
        EditorHierarchyTool::SetFocusObject(rootObject);
        EditorInspectorTool::SetFocusObject(rootObject);
    }
}

Command::EditorScene::NewGameObjectCommand::NewGameObjectCommand(std::string_view type_id, std::string_view name,
                                                                 GameObject** pOutObject)
    : 
    UmCommand("New GameObject"), 
    _typeName(type_id), 
    _newName(name), _active(true), 
    _pOutObject(pOutObject)
{
}

bool Command::EditorScene::NewGameObjectCommand::Execute()
{
    if (nullptr == UmSceneManager.GetMainScene())
    {
        return false;
    }

    if (nullptr == _newObject)
    {
        _newObject  = UmGameObjectFactory.NewGameObject(_typeName, _newName);
        _ownerScene = _newObject->GetOwnerSceneName();
        if (nullptr != _pOutObject)
        {
            *_pOutObject = _newObject.get();
            _pOutObject  = nullptr;
        }
    }
    else
    {
        _newObject->ActiveSelf = _active;
        UmSceneManager.SetObjectOwnerScene(_newObject.get(), _ownerScene);
        ESceneManager::Engine::AddGameObjectToLifeCycle(_newObject);
    }
    Scene& scene  = _newObject->GetScene();
    scene.IsDirty = true;

    return true;
}

void Command::EditorScene::NewGameObjectCommand::Undo()
{
    int    instanceID      = _newObject->GetInstanceID();
    Scene& scene           = _newObject->GetScene();
    scene.IsDirty          = true;
    _active                = _newObject->ActiveSelf;
    _newObject->ActiveSelf = false;
    _newObject->transform->DetachChildren();
    UmSceneManager.AddDestroyObjectQueue(_newObject.get());
    if (EditorHierarchyTool::GetFocusObject().lock() == _newObject)
    {
        std::weak_ptr<GameObject> empty;
        EditorHierarchyTool::SetFocusObject(empty);
    }
    if (EditorInspectorTool::GetFocusObject().lock() == _newObject)
    {
        std::weak_ptr<GameObject> empty;
        EditorInspectorTool::SetFocusObject(empty);
    }
}

Command::EditorScene::DestroyComponentCommand::DestroyComponentCommand(Component* component)
    : UmCommand("Destroy Component"), _destroyComponent(component->GetWeakPtr().lock()),
      _ownerObject(component->gameObject->GetWeakPtr()), _enable(component->Enable), _index(component->GetIndex())
{
}

Command::EditorScene::DestroyComponentCommand::~DestroyComponentCommand() {}

bool Command::EditorScene::DestroyComponentCommand::Execute()
{
    if (false == _ownerObject.expired())
    {
        UmSceneManager.AddDestroyComponentQueue(_destroyComponent.get());
        _destroyComponent->Enable = false;
    }
    return true;
}

void Command::EditorScene::DestroyComponentCommand::Undo()
{
    if (false == _ownerObject.expired())
    {
        auto owner = _ownerObject.lock();
        UmComponentFactory.InsertComponentToObject(owner.get(), _destroyComponent, _index);
        _destroyComponent->Enable = _enable;
    }
}

Command::EditorScene::AddComponentCommand::AddComponentCommand(GameObject* ownerObject, std::string_view type_id, Component** pOutComponent)
    : 
    UmCommand("AddComponent"), 
    _ownerObject(ownerObject->GetWeakPtr()), 
    _typeName(type_id), 
    _index(-1),
    _pOutComponent(pOutComponent)
{
}

bool Command::EditorScene::AddComponentCommand::Execute()
{
    if (false == _ownerObject.expired())
    {
        if (nullptr == _addComponent)
        {
            auto       pObject   = _ownerObject.lock();
            Component* component = UmComponentFactory.AddComponentToObject(pObject.get(), _typeName);
            _addComponent        = component->GetWeakPtr().lock();
            if (nullptr != _pOutComponent)
            {
                *_pOutComponent = _addComponent.get();
                _pOutComponent = nullptr;
            }
        }
        else
        {
            auto owner = _ownerObject.lock();
            UmComponentFactory.InsertComponentToObject(owner.get(), _addComponent, _index);
        }
    }
    return true;
}

void Command::EditorScene::AddComponentCommand::Undo()
{
    if (false == _ownerObject.expired())
    {
        _index = _addComponent->GetIndex();
        UmSceneManager.AddDestroyComponentQueue(_addComponent.get());
    }
}

Command::EditorScene::DuplicateCommand::DuplicateCommand(GameObject* sourceObject)
    : FocusCommand(std::weak_ptr<GameObject>(), std::weak_ptr<GameObject>(), "Duplicate")
{
    _active         = sourceObject->ActiveSelf;
    _ownerSceneName = sourceObject->GetOwnerSceneName();
    Transform::ForeachBFS(sourceObject->transform, [&](Transform* curr) 
        { 
            _sourceObjects.push_back(curr->gameObject->GetWeakPtr().lock()); 
        });
}

Command::EditorScene::DuplicateCommand::~DuplicateCommand() {}

bool Command::EditorScene::DuplicateCommand::Execute()
{
    if (true == _destObjects.empty())
    {
        GameObject* sourceRoot = _sourceObjects.front().get();
        _oldFocused = sourceRoot->GetWeakPtr();
        GameObject* destRoot = GameObject::Instantiate(sourceRoot);
        std::string destName{destRoot->Name};
        size_t pos = destName.rfind(" (");
        if (pos != std::string::npos && destName.back() == ')')
        {
            destName = destName.substr(0, pos);
        }
        destRoot->Name = GameObject::Helper::GenerateUniqueName(destName);
        _newFocused = destRoot->GetWeakPtr();
        Transform::ForeachBFS(destRoot->transform, [&](Transform* curr) 
        { 
            _destObjects.push_back(curr->gameObject->GetWeakPtr().lock()); 
        });

        if (nullptr != sourceRoot->transform->Parent)
        {
            destRoot->transform->SetParent(sourceRoot->transform->Parent, false);
        }
    }
    else
    {
        auto& rootObject = _destObjects.front();
        rootObject->ActiveSelf = _active;
        for (auto& object : _destObjects)
        {
            UmSceneManager.SetObjectOwnerScene(object.get(), _ownerSceneName);
            ESceneManager::Engine::AddGameObjectToLifeCycle(object);
        }
    }
    auto& rootObject = _destObjects.front();
    rootObject->GetScene().IsDirty = true;
    Super::Execute();

    return true;
}

void Command::EditorScene::DuplicateCommand::Undo()
{
    auto& rootObject = _destObjects.front();
    rootObject->GetScene().IsDirty = true;

    int instanceID = rootObject->GetInstanceID();
    rootObject->ActiveSelf = false;
    UmSceneManager.AddDestroyObjectQueue(rootObject.get());
    if (EditorHierarchyTool::GetFocusObject().lock() == rootObject)
    {
        std::weak_ptr<GameObject> empty;
        EditorHierarchyTool::SetFocusObject(empty);
    }
    if (EditorInspectorTool::GetFocusObject().lock() == rootObject)
    {
        std::weak_ptr<GameObject> empty;
        EditorInspectorTool::SetFocusObject(empty);
    }
    rootObject->GetScene().IsDirty = true;
    Super::Undo();
}

Command::EditorScene::PasteObjectCommand::PasteObjectCommand(std::wstring_view yamlData)
    : FocusCommand(std::weak_ptr<GameObject>(), std::weak_ptr<GameObject>(), "Paste Object")
{
    _yamlData = yamlData;
    _loadSuccess = true;
}

Command::EditorScene::PasteObjectCommand::~PasteObjectCommand() 
{

}

bool Command::EditorScene::PasteObjectCommand::Execute() 
{
    if (false == _yamlData.empty())
    {
        if (true == _destObjects.empty())
        {
            try
            {
                std::string data         = WStringToU8(_yamlData);
                YAML::Node  yamlInstance = YAML::Load(data);
                GameObject* sourceRoot   = UmGameObjectFactory.DeserializeToYaml(&yamlInstance).get();
                if (sourceRoot)
                {
                    _oldFocused = EditorHierarchyTool::GetFocusObject();
                    std::string objName(sourceRoot->Name);
                    size_t      pos = objName.rfind(" (");
                    if (pos != std::string::npos && objName.back() == ')')
                    {
                        objName = objName.substr(0, pos);
                    }
                    sourceRoot->Name = GameObject::Helper::GenerateUniqueName(objName);
                    _newFocused      = sourceRoot->GetWeakPtr();
                    Transform::ForeachBFS(sourceRoot->transform, [&](Transform* curr) {
                        _destObjects.push_back(curr->gameObject->GetWeakPtr().lock());
                    });

                    if (auto parent = EditorHierarchyTool::GetFocusObject().lock())
                    {
                        sourceRoot->transform->SetParent(parent->transform, false);
                    }
                    _loadSuccess = true;
                }
                else
                {
                    _yamlData.clear();
                    _loadSuccess = false;
                }
            }
            catch (const YAML::ParserException& e)
            {
                const char* what = e.what();
                _yamlData.clear();
                _loadSuccess = false;
            }
            catch (const YAML::Exception& e)
            {
                const char* what = e.what();
                _yamlData.clear();
                _loadSuccess = false;
            }
        }
        else
        {
            auto& rootObject       = _destObjects.front();
            rootObject->ActiveSelf = _active;
            for (auto& object : _destObjects)
            {
                UmSceneManager.SetObjectOwnerScene(object.get(), _ownerSceneName);
                ESceneManager::Engine::AddGameObjectToLifeCycle(object);
            }
        }

        if (_loadSuccess)
        {
            auto& rootObject               = _destObjects.front();
            rootObject->GetScene().IsDirty = true;
            Super::Execute();
        }
    }
    return _loadSuccess;
}

void Command::EditorScene::PasteObjectCommand::Undo() 
{
    if (false == _yamlData.empty())
    {
        auto& rootObject               = _destObjects.front();
        rootObject->GetScene().IsDirty = true;

        int instanceID         = rootObject->GetInstanceID();
        _active                = rootObject->ActiveSelf;
        _ownerSceneName        = rootObject->GetOwnerSceneName();
        rootObject->ActiveSelf = false;
        UmSceneManager.AddDestroyObjectQueue(rootObject.get());
        if (EditorHierarchyTool::GetFocusObject().lock() == rootObject)
        {
            std::weak_ptr<GameObject> empty;
            EditorHierarchyTool::SetFocusObject(empty);
        }
        if (EditorInspectorTool::GetFocusObject().lock() == rootObject)
        {
            std::weak_ptr<GameObject> empty;
            EditorInspectorTool::SetFocusObject(empty);
        }
        rootObject->GetScene().IsDirty = true;
        Super::Undo();
    }
}
