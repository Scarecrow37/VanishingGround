#include "pch.h"
#include "GraphicsBase.h"

GraphicsBase::~GraphicsBase()
{
    SetDestroy();
}

void GraphicsBase::SetDestroy()
{
    for (auto& isDestroy : _isDestroyeds)
    {
        *isDestroy = true;
    }
    _isDestroyeds.clear();
}