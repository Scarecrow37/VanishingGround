#include "pch.h"
#include "GraphicsBase.h"

void GraphicsBase::SetDestroy()
{
    for (auto& isDestroy : _isDestroyeds)
    {
        *isDestroy = true;
    }
}
