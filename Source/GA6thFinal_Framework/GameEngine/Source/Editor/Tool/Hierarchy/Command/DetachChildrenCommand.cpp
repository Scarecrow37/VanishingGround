#include "DetachChildrenCommand.h"

Command::Hierarchy::DetachChildrenCommand::DetachChildrenCommand(std::weak_ptr<GameObject> targetObject) 
    :
    UmCommand("Detach Children"),
    _targetObject(targetObject)
{
    
}

void Command::Hierarchy::DetachChildrenCommand::Execute() 
{
    if (auto targetObject = _targetObject.lock())
    {
        if (0 < targetObject->transform->ChildCount)
        {
            _childrens.clear();
            for (int i = 0; i < targetObject->transform->ChildCount; ++i)
            {
                Transform* child = targetObject->transform->GetChild(i);
                _childrens.push_back(child->gameObject->GetWeakPtr());
            }
            targetObject->transform->DetachChildren();
            targetObject->GetScene().IsDirty = true;
        }
    }      
}

void Command::Hierarchy::DetachChildrenCommand::Undo() 
{
    if (false == _targetObject.expired() && false == _childrens.empty())
    {
        std::erase_if(_childrens, [](std::weak_ptr<GameObject>& weakChild) 
        { 
            return weakChild.expired();
        });
        auto pTargetObject = _targetObject.lock();
        for (auto& weakChild : _childrens)
        {
            auto pChild = weakChild.lock();
            pChild->transform->SetParent(pTargetObject->transform);
        }
        pTargetObject->GetScene().IsDirty = true;
    }
}
