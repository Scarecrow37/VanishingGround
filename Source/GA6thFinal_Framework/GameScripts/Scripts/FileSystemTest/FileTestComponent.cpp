#include "UmFramework.h"
#include "FileTestComponent.h"

void FileTestComponent::Update()
{
    File::Guid guid = File::Path(L"Assets/Textures/AO_Noise.png");
    if (!guid.empty())
    {
        UmEngineLogger.Log(LogLevel::LEVEL_DEBUG, guid.string().c_str());
    }
    else
    {
        UmEngineLogger.Log(LogLevel::LEVEL_DEBUG, "filePath Error.");
    }
}
FileTestComponent::FileTestComponent()  = default;
FileTestComponent::~FileTestComponent() = default;
