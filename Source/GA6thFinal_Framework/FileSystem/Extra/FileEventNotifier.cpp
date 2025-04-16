#include "pch.h"
#include "FileEventNotifier.h"

namespace File
{
    FileEventNotifier::FileEventNotifier()
    {
    }

    FileEventNotifier::~FileEventNotifier() 
    {
    }

    bool FileEventNotifier::IsTriggerExtension(const FString& ext)
    {
        return _triggerExtTable.find(ext) != _triggerExtTable.end();
    }

    std::vector<FString> FileEventNotifier::GetTriggerExtensions() const
    {
        std::vector<FString> extensions;
        for (const auto& ext : _triggerExtTable)
        {
            extensions.push_back(ext);
        }
        return extensions;
    }

}
