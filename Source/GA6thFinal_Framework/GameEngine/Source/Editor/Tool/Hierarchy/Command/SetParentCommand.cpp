#include "SetParentCommand.h"

Command::Hierarchy::SetParentCommand::SetParentCommand(
    std::weak_ptr<GameObject> targetObject,
    std::weak_ptr<GameObject> prevParent,
    std::weak_ptr<GameObject> currParent)
    :
    UmCommand("SetParent"),
    _targetObject(targetObject),
    _prevParent(prevParent),
    _currParent(currParent),
    _isPrevRoot(false), 
    _isCurrRoot(false)
{

}

Command::Hierarchy::SetParentCommand::SetParentCommand(
    std::weak_ptr<GameObject> targetObject,
    const std::nullptr_t& prevNull,
    std::weak_ptr<GameObject> currParent) 
    :
    UmCommand("SetParent"),
    _targetObject(targetObject), 
    _currParent(currParent), 
    _isPrevRoot(true),
    _isCurrRoot(false)
{

}

Command::Hierarchy::SetParentCommand::SetParentCommand(
    std::weak_ptr<GameObject> targetObject,                                                     
    std::weak_ptr<GameObject> prevParent,                                                     
    const std::nullptr_t&     currNull)
    :
     UmCommand("SetParent"),
     _targetObject(targetObject),
     _prevParent(prevParent),
     _isPrevRoot(false), 
     _isCurrRoot(true)
{

}

void Command::Hierarchy::SetParentCommand::Execute() 
{
    if (false == _targetObject.expired())
    {
        auto target = _targetObject.lock();
        if (true == _isCurrRoot)
        {
            if (nullptr != target->transform->Parent)
            {
                target->transform->SetParent(nullptr);
                target->GetScene().IsDirty = true;
            }
        }
        else if (false == _currParent.expired())
        {
            auto curr = _currParent.lock();
            if (&curr->transform != target->transform->Parent)
            {
                target->transform->SetParent(curr->transform);
                target->GetScene().IsDirty = true;
            }
        }
    }
}

void Command::Hierarchy::SetParentCommand::Undo() 
{
    if (false == _targetObject.expired())
    {
        auto target = _targetObject.lock();
        if (true == _isPrevRoot)
        {
            if (nullptr != target->transform->Parent)
            {
                target->transform->SetParent(nullptr);
                target->GetScene().IsDirty = true;
            }
        }
        else if (false == _prevParent.expired())
        {
            auto prev = _prevParent.lock();
            if (&prev->transform != target->transform->Parent)
            {
                target->transform->SetParent(prev->transform);
                target->GetScene().IsDirty = true;
            }
        }
    }
}
