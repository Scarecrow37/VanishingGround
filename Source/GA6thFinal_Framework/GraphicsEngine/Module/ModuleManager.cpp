#include "pch.h"
#include "ModuleManager.h"

#include "AccumulationModule.h"
#include "GaussianBlurModule.h"

ModuleManager::ModuleManager() = default;

ModuleManager::~ModuleManager() = default;

void ModuleManager::Initialize()
{
    _modules.push_back(std::move(std::make_unique<GaussianBlurModule>()));
    _modules.back()->Initialize();

    _modules.push_back(std::move(std::make_unique<AccumulationModule>()));
    _modules.back()->Initialize();
}