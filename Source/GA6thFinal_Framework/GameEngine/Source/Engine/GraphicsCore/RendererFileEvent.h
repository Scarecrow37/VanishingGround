#pragma once
#include "../FileSystem/Extra/FileEventSubscriber.h"

class RendererFileEvent : public File::FileEventSubscriber
{
public:
    RendererFileEvent() = default;
    ~RendererFileEvent() = default;

public:
    void OnFileRegistered(const File::Path& path) override;
    void OnFileModified(const File::Path& path) override;
    void OnRequestedInspect(const File::Path& path) override;
};