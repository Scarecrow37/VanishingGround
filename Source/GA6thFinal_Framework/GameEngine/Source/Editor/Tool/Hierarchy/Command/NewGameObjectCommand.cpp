#include "NewGameObjectCommand.h"

Command::Hierarchy::NewGameObjectCommand::NewGameObjectCommand(std::string_view type_id, std::string_view name) 
    :
    UmCommand("New GameObject"),
    _typeName(type_id),
    _newName(name)
{

}

void Command::Hierarchy::NewGameObjectCommand::Execute() 
{
    auto pObject = UmGameObjectFactory.NewGameObject(_typeName, _newName);
    pObject->GetScene().IsDirty = true;
    _newObject = pObject;
}

void Command::Hierarchy::NewGameObjectCommand::Undo() 
{
    if (false == _newObject.expired())
    {
        auto pObject = _newObject.lock();
        GameObject::Destroy(pObject.get());
        pObject->GetScene().IsDirty = true;
    }
}
