#include "pch.h"
#include "Component.h"

unsigned int Component::_globalID = 0;

Component::Component()
	: _ID(_globalID++)
{

}