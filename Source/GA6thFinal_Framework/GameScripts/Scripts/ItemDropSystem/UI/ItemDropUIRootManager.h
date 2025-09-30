#pragma once
#include "UmFramework.h"
#include "ItemDropSystem/Interface/IDropItem.h"
#include "Utility/SingletonHelper.h"

class ItemDropUIRootManager : public Component
{
    USING_PROPERTY(ItemDropUIRootManager)
public:
    inline static constexpr const char* TAG = "Item Drop UI Root";
    ItemDropUIRootManager();
    ~ItemDropUIRootManager() override;

public:
    REFLECT_PROPERTY(
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
    void ImGuiDrawPropertysEvent() override;

    void ImGuiDrawArtifactUIAssetSetting();

    void Reset() override;
    void Awake() override;
    void Start() override;

private:
    SingletonComponent<ItemDropUIRootManager> _singletonComponent{this};
};
