#include "pch.h"
#include "DropPrefabCommand.h"

Command::Hierarchy::DropPrefabCommand::DropPrefabCommand(const File::Guid& guid) 
    :
    UmCommand("Make Prefab Instance"),
    _guid(guid)
{

}

void Command::Hierarchy::DropPrefabCommand::Execute() 
{
    _newObject = UmGameObjectFactory.DeserializeToGuid(_guid);
    UmSceneManager.GetMainScene()->IsDirty = true;
}

void Command::Hierarchy::DropPrefabCommand::Undo() 
{
    if (false == _newObject.expired())
    {
        auto pObject = _newObject.lock();
        GameObject::Destroy(pObject.get());
        pObject->GetScene().IsDirty = true;
    }
}
