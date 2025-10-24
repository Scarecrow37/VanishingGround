#pragma once
#include "UmFramework.h"
#include "Interface/IDropItem.h"
#include "Utility/SingletonHelper.h"

class ItemDropSystem : public Component
{
    USING_PROPERTY(ItemDropSystem)
public:
    inline static const std::string WATCHER_KEY = "8940F3B8-1D49-4556-9588-5D423CAC794D";
    static const size_t ARTIFACT_TYPE_COUNT; // 유물 카테고리 개수
    inline static constexpr int ITEM_DROP_RATE_BONUS_MAX = 5; //아이템 드롭 확률 보너스 최대 개수

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

    /// <summary>
    /// 스테이지 클리어 UI 연출을 실행합니다.
    /// </summary>
    void PlayItemDropUISequence();

    /// <summary>
    /// UI 모델을 Notify 합니다.
    /// </summary>
    void NotifyUIModel() { _dropItemsModel.Notify(); }

    /// <summary>
    /// 해당 인덱스의 유물이 획득한 보상인지 확인합니다. SetDropItem이 호출될때마다 초기화됩니다.
    /// </summary>
    /// <param name="index :">해당 아티팩트 인덱스</param>
    /// <returns>잘못된 인덱스를 넣어도 true를 반환합니다.</returns>
    bool IsObtainArtifact(size_t index) const 
    {
        if (index < _obtainArtifactFlag.size())
        {
            return _obtainArtifactFlag[index];
        }
        return true;
    }

    /// <summary>
    /// 전달한 인덱스의 유물 획득 플래그를 활성화합니다. SetDropItem이 호출될때마다 초기화됩니다.
    /// </summary>
    /// <param name="index :">해당 아티팩트 인덱스</param>
    void SetObtainArtifact(size_t index)
    {
        if (index < _obtainArtifactFlag.size())
        {
            _obtainArtifactFlag[index] = true;
        }
    }

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
    std::array<std::vector<double>, ITEM_DROP_RATE_BONUS_MAX> WeaponGradeWeight;     // 무기 등급별 가중치
    std::array<std::vector<double>, ITEM_DROP_RATE_BONUS_MAX> RevelationGradeWeight; // 계시 등급별 가중치
    std::array<std::vector<double>, ITEM_DROP_RATE_BONUS_MAX> AccessoryGradeWeight;  // 장신구 등급별 가중치
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

    // 장신구 드랍 보정 확률 조절용
    void ImGuiDrawAccessoryGradeWeight();

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

    void Reset() override;
    void Awake() override;
    void Update() override;
    void OnDestroy() override;

    void DebugUpdate();

private:
    SingletonComponent<ItemDropSystem>     _singletonComponent{this};
    MVVM::Model<std::vector<DropItemInfo>> _dropItemsModel;
    int _stageClearCount = 0;
    int _itemDropRateBonus = 0;
    std::array<bool, ARTIFACT_DROP_COUNT>  _obtainArtifactFlag{}; // 이번 보상 획득 여부 플래그
};
