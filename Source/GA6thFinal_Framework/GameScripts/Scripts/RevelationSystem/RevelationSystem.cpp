#include "pchScripts.h"
#include "RevelationSystem.h"
RevelationSystem::RevelationSystem() = default;
RevelationSystem::~RevelationSystem() = default;

void RevelationSystem::Reset() 
{
    static_instance = this;
}
