#include "pch.h"
#include "AudioModule.h"

void AudioModule::PreInitialize() {}

void AudioModule::ModuleInitialize()
{
    UmFileSystem.RegisterFileEventSubscriber(this, {AUDIO_EXTENSION});
    UmAudio.Initialize();
}

void AudioModule::PreUnInitialize() {}

void AudioModule::ModuleUnInitialize()
{
    UmAudio.Finalize();
}