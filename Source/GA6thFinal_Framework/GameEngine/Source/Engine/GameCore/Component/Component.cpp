#include "pch.h"

Component::Component() :
    _className(),
    _gameObect(nullptr)
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
    for (size_t i = 0; i < GetComponentCount(); i++)
    {
        if (Component* curr = GetComponentAtIndex<Component>(i))
        {
            if (curr == this)
            {
                return i;
            }
        }
    }
    return -1;
}