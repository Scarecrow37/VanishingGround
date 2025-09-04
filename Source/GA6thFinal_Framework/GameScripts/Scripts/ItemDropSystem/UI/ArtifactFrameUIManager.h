#pragma once
#include "UmFramework.h"
class ArtifactFrameUIManager : public Component
{
    USING_PROPERTY(ArtifactFrameUIManager)
    inline static ArtifactFrameUIManager* static_instance = nullptr;

public:
    inline static constexpr const char* TAG = "Artifact Frame UI Manager";

    static ArtifactFrameUIManager* GetInstance(std::source_location location = std::source_location::current()) 
    {
        if (false == static_instance->gameObject->IsValid())
        {
            static_instance = nullptr;
        }

        if (static_instance)
        {
            return static_instance;
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"ArtifactFrameUIManager가 존재하지 않습니다.", location);
            return nullptr;
        }
    }
    ArtifactFrameUIManager();
    ~ArtifactFrameUIManager() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(ArtifactFrameUIManager)

    void Reset() override;
    void Awake() override;
};
