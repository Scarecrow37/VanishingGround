#pragma once
#include "../FileSystem/Event/FileEventSubscriber.h"

class RendererFileEvent : public File::FileEventSubscriber
{
public:
    RendererFileEvent() = default;
    ~RendererFileEvent() = default;

public:
    void OnPostRequestedSave() override;
    void OnPostRequestedLoad() override;
};