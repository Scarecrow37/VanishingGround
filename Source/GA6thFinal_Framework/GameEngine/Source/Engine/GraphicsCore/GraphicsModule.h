#pragma once

class GraphicsModule : public IAppModule
{
public:
    GraphicsModule() = default;
    virtual ~GraphicsModule() = default;

public:
    // IAppModule을(를) 통해 상속됨
    void PreInitialize() override;
    void ModuleInitialize() override;
    void PreUnInitialize() override;
    void ModuleUnInitialize() override;

};
