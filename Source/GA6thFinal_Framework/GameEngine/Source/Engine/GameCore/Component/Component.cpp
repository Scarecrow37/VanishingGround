#include "pch.h"

Component::Component() :
    _className(),
    _gameObect(nullptr),
    _index(-1)
{

}

Component::~Component() = default;

Component::InitFlags::InitFlags() :
    _isAwake(false),
    _isStart(false)
{
}

Component::InitFlags::~InitFlags() = default;
