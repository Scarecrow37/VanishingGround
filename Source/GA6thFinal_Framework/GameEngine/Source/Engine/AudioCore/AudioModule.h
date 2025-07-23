#pragma once

class AudioModule final : public IAppModule, public File::FileEventSubscriber
{
    static constexpr const char* AUDIO_EXTENSION = ".wav";

public:
    void PreInitialize() override;
    void ModuleInitialize() override;
    void PreUnInitialize() override;
    void ModuleUnInitialize() override;
};