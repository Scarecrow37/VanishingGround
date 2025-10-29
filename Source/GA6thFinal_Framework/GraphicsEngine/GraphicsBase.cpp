#include "pch.h"
#include "GraphicsBase.h"

unsigned long long GraphicsBase::_globalID = 0;

GraphicsBase::GraphicsBase()
    : _ID(++_globalID)
    , _isActive(nullptr)
    , _referenceCount(0)
{
}

void GraphicsBase::AddReference()
{
    ++_referenceCount;
}

void GraphicsBase::Release()
{
    if (_referenceCount > 0)
        --_referenceCount;

    if (0 == _referenceCount)
    {
        Global::renderer->AddToBeReleasedComponent(this);
    }
}