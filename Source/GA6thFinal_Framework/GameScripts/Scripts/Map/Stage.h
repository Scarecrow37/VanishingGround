#pragma once
#include "ItemDropSystem/Interface/IDropItem.h"
#include "Monster/Common/MonsterCommon.h"

class Stage : public UISFXNavigationComponent
{
    USING_PROPERTY(Stage)

public:
    Stage();
    ~Stage() override;

public:
    const std::array<int, ARTIFACT_DROP_COUNT>& GetDropItems() const { return _dropItemAssetIDs; }
    bool                                        IsEnable() const { return _stageEnable.Get(); }

public:
    void RegisterStage(const std::string& key, const File::Guid& enableImage, const File::Guid& disableImage);
    void UpdateData(const std::string& key, const File::Guid& enableImage, const File::Guid& disableImage);

public:
    void FocusIn(FocusCallType callType) override;
    void Submit() override;

public:
    void Start() override;

public:
    REFLECT_PROPERTY(StagePath, LightingPath, MainLevel, SubLevel, BattleCount)

    GETTER_ONLY(std::string, StagePath) { return File::Guid(ReflectFields->StageGuid).ToPath().string(); }
    PROPERTY(StagePath)

    GETTER_ONLY(std::string, LightingPath) { return File::Guid(ReflectFields->LightingGuid).ToPath().string(); }
    PROPERTY(LightingPath)

    SETTER(int, MainLevel) { ReflectFields->MainLevel = value; }
    GETTER(int, MainLevel) { return ReflectFields->MainLevel; }
    PROPERTY(MainLevel)

    SETTER(int, SubLevel) { ReflectFields->SubLevel = value; }
    GETTER(int, SubLevel) { return ReflectFields->SubLevel; }
    PROPERTY(SubLevel)

    SETTER(int, BattleCount) { _battleCount = value; }
    GETTER(int, BattleCount) { return _battleCount; }
    PROPERTY(BattleCount)

protected:
    REFLECT_FIELDS_BEGIN(UISFXNavigationComponent)
    std::string StageGuid;
    std::string LightingGuid;
    int         MainLevel = 0; // ex) 맵이 1-3일 때 1부분
    int         SubLevel = 0;  // ex) 맵이 1-3일 때 3부분
    REFLECT_FIELDS_END(Stage)

private:
    std::string                                   _key;
    MVVM::Model<bool>                             _stageEnable      = true;

    std::array<int, ARTIFACT_DROP_COUNT>          _dropItemAssetIDs = {0, 0, 0, 0, 0, 0};
    std::array<DropItemInfo, ARTIFACT_DROP_COUNT> _dropItemInfos;

    int _battleCount = 1;
};