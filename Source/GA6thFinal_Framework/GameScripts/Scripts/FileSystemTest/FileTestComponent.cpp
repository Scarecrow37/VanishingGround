#include "UmFramework.h"
#include "FileTestComponent.h"

void FileTestComponent::Update()
{
    File::Guid guid = File::Path(ReflectFields->filePath);
    if (!guid.empty())
    {
        ReflectFields->fileGuid = guid.string();
    }
    else
    {
        ReflectFields->fileGuid = "null";
    }
}
FileTestComponent::FileTestComponent()  = default;
FileTestComponent::~FileTestComponent() = default;
