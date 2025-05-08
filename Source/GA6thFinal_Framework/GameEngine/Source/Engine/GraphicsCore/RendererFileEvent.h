#pragma once
#include "../FileSystem/Extra/FileEventNotifier.h"

class RendererFileEvent : public File::FileEventNotifier
{
public:
    RendererFileEvent() = default;
    ~RendererFileEvent() = default;

public:
    void OnFileRegistered(const File::Path& path) override;
    void OnFileModified(const File::Path& path) override;
    void OnRequestedInspect(const File::Path& path) override;
};