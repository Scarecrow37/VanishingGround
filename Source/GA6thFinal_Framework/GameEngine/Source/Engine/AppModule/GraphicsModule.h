#pragma once

class GraphicsModule : public IAppModule
{
public:
    GraphicsModule();
    ~GraphicsModule() override;

public:
    void PreInitialize() override;
    void ModuleInitialize() override;

    void PreUnInitialize() override;
    void ModuleUnInitialize() override;

private:
    std::unique_ptr<class RendererFileEvent> _rendererFileEvent;
};