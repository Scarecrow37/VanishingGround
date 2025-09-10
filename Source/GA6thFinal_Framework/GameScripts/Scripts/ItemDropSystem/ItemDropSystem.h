#pragma once
#include "UmFramework.h"
#include "Interface/IDropItem.h"

class ItemDropSystem : public Component
{
    USING_PROPERTY(ItemDropSystem)
    inline static ItemDropSystem* static_instance = nullptr;
public:
    inline static const std::string WATCHER_KEY = "8940F3B8-1D49-4556-9588-5D423CAC794D";
    static const size_t ARTIFACT_TYPE_COUNT; // 유물 카테고리 개수
    inline static constexpr int ITEM_DROP_RATE_BONUS_MAX = 5; //아이템 드롭 확률 보너스 최대 개수
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

    /// <summary>
    /// 현재 스테이지의 클리어 횟수를 설정합니다. 해당 클리어 수에 맞게 선택할 수 있는 UI가 갱신됩니다.
    /// </summary>
    void SetStageClearCount(int count);

public:
    REFLECT_PROPERTY(
        StageClearCount,
        ItemDropRateBonus
    )
    
    GETTER(int, StageClearCount) { return _stageClearCount; }
    SETTER(int, StageClearCount) { SetStageClearCount(value); }
    // 스테이지 클리어 횟수에 따른 보상 UI를 갱신합니다.
    // type : int
    PROPERTY(StageClearCount)

    GETTER(int, ItemDropRateBonus) { return _itemDropRateBonus; }
    SETTER(int, ItemDropRateBonus) { _itemDropRateBonus = std::clamp(value, 0, ITEM_DROP_RATE_BONUS_MAX); }
    // 아이템 드랍 확률 보정 수치입니다.
    // type : int
    PROPERTY(ItemDropRateBonus)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::vector<int> MaxDropCount; //카테고리별 드랍 가능한 최대 갯수
    std::array<std::vector<double>, ITEM_DROP_RATE_BONUS_MAX> WeaponGradeWeight; // 무기 등급별 가중치
    std::array<std::vector<double>, ITEM_DROP_RATE_BONUS_MAX> RevelationGradeWeight; // 계시 등급별 가중치
    REFLECT_FIELDS_END(ItemDropSystem)

protected:
    void ImGuiDrawPropertysEvent() override;

    // 유물 드랍뽑기를 테스트합니다.
    void ImGuiDrawTestRollArtifacts();

    // 카테고리별 드랍 가능 갯수 조정용 에디터 함수
    void ImGuiDrawMaxDropCount(); 

    // 무기 드랍 보정 확률 조절용
    void ImGuiDrawWeaponGradeWeight();

    // 계시 드랍 보정 확률 조절용
    void ImGuiDrawRevelationGradeWeight();

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

    void Reset() override;
    void Awake() override;

private:
    MVVM::Model<std::vector<DropItemInfo>> _dropItemsModel;
    int _stageClearCount = 0;
    int _itemDropRateBonus = 0;
};
