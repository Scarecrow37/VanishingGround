#pragma once
#include "UmFramework.h"
#include "Interface/IDropItem.h"

/* 유물 보상에 등장 가능한 아이템 타입 */
enum class ArtifactDropType
{
    SWORD,     // 검
    DAGGER,    // 단검
    WARHAMMER, // 대형 망치
    REVELATION // 계시
};

class ItemDropSystem : public Component
{
    USING_PROPERTY(ItemDropSystem)
    inline static ItemDropSystem* static_instance = nullptr;
public:
    static const size_t ARTIFACT_TYPE_COUNT; // 유물 카테고리 개수
    static ItemDropSystem* GetInstance(std::source_location location = std::source_location::current()) 
    { 
        if (nullptr == static_instance)
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"ItemDropSystem가 존재하지 않습니다.", location);
        }
        return static_instance; 
    }

    ItemDropSystem();
    ~ItemDropSystem() override;

public:
    /// <summary>
    /// Artifact 아이템들을 랜덤으로 뽑아서 정보들을 반환합니다.
    /// </summary>
    std::array<DropItemInfo, ARTIFACT_DROP_COUNT> RollArtifacts();

    /// <summary>
    /// 현재 보상 아이템을 설정합니다. UI도 갱신됩니다.
    /// </summary>
    /// <param name="itemInfos :"></param>
    void SetDropItem(const std::array<DropItemInfo, ARTIFACT_DROP_COUNT>& itemInfos);

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::vector<int> MaxDropCount; //카테고리별 드랍 가능한 최대 갯수
    REFLECT_FIELDS_END(ItemDropSystem)

protected:
    void ImGuiDrawPropertysEvent() override;

    // 카테고리별 드랍 가능 갯수 조정용 에디터 함수
    void ImGuiDrawMaxDropCount(); 
    // 유물 드랍뽑기를 테스트합니다.
    void ImGuiDrawTestRollArtifacts();

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

    void Reset() override;
    void Awake() override;

private:
    MVVM::Model<std::vector<DropItemInfo>> _dropItemsModel;


};
