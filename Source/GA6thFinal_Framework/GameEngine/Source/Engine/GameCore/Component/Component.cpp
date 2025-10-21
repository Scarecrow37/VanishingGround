#include "pch.h"

REFLECT_FUNCTION(Component)

Component::Component(TYPE type)
    : 
    _className(), 
    _gameObject(&staticDummyObject),
    _type(type), 
    _enableInHierarchy(true), 
    _prevFrameEnableInHierarchy(true)
{

}

Component::~Component()
{
    UnsetOverrideFlags();
}

Component::InitFlags::InitFlags() 
    :
    _isAwake(false),
    _isStart(false)
{

}

Component::InitFlags::~InitFlags() = default;

int Component::GetIndex() const 
{
    return gameObject->GetComponentIndex(this);
}

std::weak_ptr<ITimeInvoker> Component::GetWeakInvoker()
{
    auto ptr = GetWeakPtr().lock();
    return std::weak_ptr<ITimeInvoker>(ptr);
}

void Component::UpdateEnableInHierarchy()
{
    _enableInHierarchy = gameObject->ActiveInHierarchy && ReflectFields->_enable;
}
