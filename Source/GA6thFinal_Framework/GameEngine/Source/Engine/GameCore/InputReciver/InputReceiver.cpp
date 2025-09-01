#include "pch.h"

InputReceiver::~InputReceiver() 
{
    if (nullptr != _isDestroy)
    {
        *_isDestroy = true;
    }
}
