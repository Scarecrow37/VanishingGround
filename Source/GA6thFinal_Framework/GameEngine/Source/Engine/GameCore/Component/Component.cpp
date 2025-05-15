#include "pch.h"

Component::Component(Type type)
    : 
    _className(), 
    _gameObect(nullptr),
    _type(type)
{

}

Component::~Component() = default;

Component::InitFlags::InitFlags() :
    _isAwake(false),
    _isStart(false)
{

}

Component::InitFlags::~InitFlags() = default;

int Component::GetIndex() const 
{
    return gameObject->GetComponentIndex(this);
}