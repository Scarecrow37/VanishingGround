#pragma once

class EngineCoresModule : public IAppModule
{
    virtual void PreInitialize();
    virtual void ModuleInitialize() {}

    virtual void PreUnInitialize();
    virtual void ModuleUnInitialize() {}
};