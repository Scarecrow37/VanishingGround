#pragma once
#include "ItemDropSystem/Interface/IDropItem.h"

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
    REFLECT_PROPERTY(StagePath, LightingPath)

    GETTER_ONLY(std::string, StagePath) { return ReflectFields->StagePath; }
    PROPERTY(StagePath)

    GETTER_ONLY(std::string, LightingPath) { return ReflectFields->LightingPath; }
    PROPERTY(LightingPath)

protected:
    REFLECT_FIELDS_BEGIN(UISFXNavigationComponent)
    std::string StagePath;
    std::string LightingPath;
    REFLECT_FIELDS_END(Stage)

private:
    MVVM::Model<bool>                             _stageEnable      = true;
    std::array<int, ARTIFACT_DROP_COUNT>          _dropItemAssetIDs = {0, 0, 0, 0, 0, 0};
    std::array<DropItemInfo, ARTIFACT_DROP_COUNT> _dropItemInfos;
    std::string                                   _key;
};