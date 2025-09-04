#pragma once
#include "UmFramework.h"
class ItemDropUIRootManager : public Component
{
    USING_PROPERTY(ItemDropUIRootManager)
    inline static ItemDropUIRootManager* static_instance = nullptr;
public:
    inline static constexpr const char* TAG = "Item Drop UI Root";

    /// <summary>
    /// ItemDropUIRootManager가 존재하면 포인터를 반환합니다.
    /// </summary>
    /// <param name="location :">기본 인자를 통해 해당 함수를 호출한 위치를 기록합니다.</param>
    /// <returns>없으면 nullptr</returns>
    static ItemDropUIRootManager* GetInstance(std::source_location location = std::source_location::current()) 
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
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Item Drop UI Root가 존재하지 않습니다.", location);
            return nullptr;
        }
    }

    ItemDropUIRootManager();
    ~ItemDropUIRootManager() override;

public:
    REFLECT_PROPERTY(
        ArtifactsUIFrameAsset
    )

    GETTER_ONLY(std::string, ArtifactsUIFrameAsset)
    {
        File::Guid  guid = ReflectFields->ArtifactsUIFrameAssetGuid;
        std::string path = guid.ToPath().string();
        return path;
    }
    //type : std::string
    //유물 드랍 프레임 UI 에셋 경로입니다.
    PROPERTY(ArtifactsUIFrameAsset)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string ArtifactsUIFrameAssetGuid;
    REFLECT_FIELDS_END(ItemDropUIRootManager)

    void DeserializedReflectEvent() override;
    void Reset() override;
    void Awake() override;
};
