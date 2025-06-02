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

void Command::EditorScene::DestroyGameObjectCommand::Execute()
{
    auto& rootObject               = _destroyObjects.front();
    rootObject->GetScene().IsDirty = true;

    int instanceID         = rootObject->GetInstanceID();
    rootObject->ActiveSelf = false;
    UmSceneManager.AddDestroyObjectQueue(rootObject.get());
    if (EditorHierarchyTool::HierarchyFocusObjWeak.lock() == rootObject)
    {
        std::weak_ptr<GameObject> empty;
        EditorHierarchyTool::HierarchyFocusObjWeak = empty;
        EditorInspectorTool::SetFocusObject(empty);
        _isFocus = true;
    }
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
        EditorHierarchyTool::HierarchyFocusObjWeak = rootObject;
        EditorInspectorTool::SetFocusObject(rootObject);
    }
}

Command::EditorScene::NewGameObjectCommand::NewGameObjectCommand(std::string_view type_id, std::string_view name)
    : UmCommand("New GameObject"), _typeName(type_id), _newName(name), _active(true)
{
}

void Command::EditorScene::NewGameObjectCommand::Execute()
{
    if (nullptr == _newObject)
    {
        _newObject  = UmGameObjectFactory.NewGameObject(_typeName, _newName);
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

void Command::EditorScene::NewGameObjectCommand::Undo()
{
    int    instanceID      = _newObject->GetInstanceID();
    Scene& scene           = _newObject->GetScene();
    scene.IsDirty          = true;
    _active                = _newObject->ActiveSelf;
    _newObject->ActiveSelf = false;
    _newObject->transform->DetachChildren();
    UmSceneManager.AddDestroyObjectQueue(_newObject.get());
    if (EditorHierarchyTool::HierarchyFocusObjWeak.lock() == _newObject)
    {
        std::weak_ptr<GameObject> empty;
        EditorHierarchyTool::HierarchyFocusObjWeak = empty;
        EditorInspectorTool::SetFocusObject(empty);
    }
}

Command::EditorScene::DestroyComponentCommand::DestroyComponentCommand(Component* component)
    : UmCommand("Destroy Component"), _destroyComponent(component->GetWeakPtr().lock()),
      _ownerObject(component->gameObject->GetWeakPtr()), _enable(component->Enable), _index(component->GetIndex())
{
}

Command::EditorScene::DestroyComponentCommand::~DestroyComponentCommand() {}

void Command::EditorScene::DestroyComponentCommand::Execute()
{
    if (false == _ownerObject.expired())
    {
        UmSceneManager.AddDestroyComponentQueue(_destroyComponent.get());
    }
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

Command::EditorScene::AddComponentCommand::AddComponentCommand(GameObject* ownerObject, std::string_view type_id)
    : UmCommand("AddComponent"), _ownerObject(ownerObject->GetWeakPtr()), _typeName(type_id), _index(-1)
{
}

void Command::EditorScene::AddComponentCommand::Execute()
{
    if (false == _ownerObject.expired())
    {
        if (nullptr == _addComponent)
        {
            auto       pObject   = _ownerObject.lock();
            Component* component = UmComponentFactory.AddComponentToObject(pObject.get(), _typeName);
            _addComponent        = component->GetWeakPtr().lock();
        }
        else
        {
            auto owner = _ownerObject.lock();
            UmComponentFactory.InsertComponentToObject(owner.get(), _addComponent, _index);
        }
    }
}

void Command::EditorScene::AddComponentCommand::Undo()
{
    if (false == _ownerObject.expired())
    {
        _index = _addComponent->GetIndex();
        UmSceneManager.AddDestroyComponentQueue(_addComponent.get());
    }
}

Command::EditorScene::InstantiateCommand::InstantiateCommand(GameObject* sourceObject)
    : FocusCommand(std::weak_ptr<GameObject>(), std::weak_ptr<GameObject>())
{
    _active         = sourceObject->ActiveSelf;
    _ownerSceneName = sourceObject->GetOwnerSceneName();
    Transform::ForeachBFS(sourceObject->transform, [&](Transform* curr) 
        { 
            _sourceObjects.push_back(curr->gameObject->GetWeakPtr().lock()); 
        });
}

Command::EditorScene::InstantiateCommand::~InstantiateCommand() {}

void Command::EditorScene::InstantiateCommand::Execute()
{
    if (true == _destObjects.empty())
    {
        GameObject* sourceRoot = _sourceObjects.front().get();
        _oldFocused = sourceRoot->GetWeakPtr();
        GameObject* destRoot = GameObject::Instantiate(sourceRoot);
        destRoot->Name = GameObject::Helper::GenerateUniqueName(destRoot->Name);
        _newFocused = destRoot->GetWeakPtr();
        Transform::ForeachBFS(destRoot->transform, [&](Transform* curr) 
            { 
                _destObjects.push_back(curr->gameObject->GetWeakPtr().lock()); 
            });
    }
    else
    {
        auto& rootObject       = _destObjects.front();
        rootObject->ActiveSelf = _active;
        for (auto& object : _destObjects)
        {
            UmSceneManager.SetObjectOwnerScene(object.get(), _ownerSceneName);
            rootObject->GetScene().IsDirty = true;
            ESceneManager::Engine::AddGameObjectToLifeCycle(object);
        }
    }
    Super::Execute();
}

void Command::EditorScene::InstantiateCommand::Undo()
{
    auto& rootObject               = _destObjects.front();
    rootObject->GetScene().IsDirty = true;

    int instanceID         = rootObject->GetInstanceID();
    rootObject->ActiveSelf = false;
    UmSceneManager.AddDestroyObjectQueue(rootObject.get());
    if (EditorHierarchyTool::HierarchyFocusObjWeak.lock() == rootObject)
    {
        std::weak_ptr<GameObject> empty;
        EditorHierarchyTool::HierarchyFocusObjWeak = empty;
        EditorInspectorTool::SetFocusObject(empty);
    }
    Super::Undo();
}
