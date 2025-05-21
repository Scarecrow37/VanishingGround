#include "pch.h"
#include "FileEventSubscriber.h"

namespace File
{
    FileEventSubscriber::FileEventSubscriber()
    {
    }

    FileEventSubscriber::~FileEventSubscriber() 
    {
    }

    bool FileEventSubscriber::IsTriggerExtension(const FString& ext)
    {
        return _triggerExtTable.find(ext) != _triggerExtTable.end();
    }

    std::vector<FString> FileEventSubscriber::GetTriggerExtensions() const
    {
        std::vector<FString> extensions;
        for (const auto& ext : _triggerExtTable)
        {
            extensions.push_back(ext);
        }
        return extensions;
    }

}
