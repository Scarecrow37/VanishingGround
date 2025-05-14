#include "pch.h"
#include "PackPrefabCommand.h"

Command::Hierarchy::PackPrefabCommand::PackPrefabCommand(std::weak_ptr<GameObject> targetObject, const File::Guid& guid)
    :
    UmCommand("Pack Prefab"), 
    _targetObject(targetObject),
    _currGuid(guid)
{
    auto pObject = targetObject.lock();
    File::Path path = (std::string)pObject->PrefabPath;
    _prevGuid = path.ToGuid();
}

void Command::Hierarchy::PackPrefabCommand::Execute() 
{
    if (false == _targetObject.expired())
    {
        auto pTarget = _targetObject.lock();
        if (_currGuid != File::NULL_GUID)
        {
            UmGameObjectFactory.UnpackPrefab(pTarget.get());
            UmGameObjectFactory.PackPrefab(pTarget.get(), _currGuid);
            pTarget->GetScene().IsDirty = true;
        }
        else
        {
            UmGameObjectFactory.UnpackPrefab(pTarget.get());
            pTarget->GetScene().IsDirty = true;
        }
    }
}

void Command::Hierarchy::PackPrefabCommand::Undo() 
{
    if (false == _targetObject.expired())
    {
        auto pTarget = _targetObject.lock();
        if (_prevGuid != File::NULL_GUID)
        {
            UmGameObjectFactory.UnpackPrefab(pTarget.get());
            UmGameObjectFactory.PackPrefab(pTarget.get(), _prevGuid);
            pTarget->GetScene().IsDirty = true;
        }
        else
        {
            UmGameObjectFactory.UnpackPrefab(pTarget.get());
            pTarget->GetScene().IsDirty = true;
        }
    }
}
