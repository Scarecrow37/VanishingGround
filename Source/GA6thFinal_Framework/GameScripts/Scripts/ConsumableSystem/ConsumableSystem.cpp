#include "pchScripts.h"
#include "ConsumableSystem.h"

UMREAL_COMPONENT(ConsumableSystem)

ConsumableSystem::ConsumableSystem()
{

}

ConsumableSystem::~ConsumableSystem()
{

}

void ConsumableSystem::Reset() 
{
    _singletonComponent.SetSingleTon();
}

void ConsumableSystem::Awake() 
{
    if (_singletonComponent.TrySingleTon())
    {

    }
}

void ConsumableSystem::ImGuiDrawPropertysEvent() 
{

}
