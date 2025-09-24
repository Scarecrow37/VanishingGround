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

    /// <summary>
    /// 유물 드랍 타입에 따른 에셋 아이디를 반환합니다.
    /// </summary>
    /// <param name="artifactDropType :">가져올 아이디</param>
    /// <returns>실패시 0</returns>
    int GetArtifactCategoryAssetID(ArtifactDropType artifactDropType);

    /// <summary>
    /// 아이템 ID를 통해 아이콘 ID를 반환합니다.
    /// </summary>
    /// <param name="info :">아이템 정보</param>
    /// <returns>아이콘 ID</returns>
    int GetArtifactIconID(DropItemInfo info);

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string ArtifactsUIFrameAssetGuid;
    std::vector<int> ArtifactsCategoryAssetID;
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
