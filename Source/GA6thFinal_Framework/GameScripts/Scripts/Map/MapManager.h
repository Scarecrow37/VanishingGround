#pragma once
#include "Map/StageData.h"
#include "Monster/Common/MonsterCommon.h"
#include "Utility/SingletonHelper.h"

class OpenInventoryComponent;
class Stage;
class ScrollingWrapper;
class ImageElement;

class MapManager : public Component, public InputReceiver
{
    enum AssetIDs { BACKGROUND, STAGE_ENABLE, STAGE_DISABLE, STAGE_FOCUS, REWARD_POPUP, MAX };
    USING_PROPERTY(MapManager)

public:
    MapManager();
    ~MapManager() override;

private:
    void Awake() override;
    void Update() override;
    void OnEnable() override;
    void OnLoadScene(Scene& loadScene, LoadSceneMode mode) override;

    void FindUI();

public:
    void UINotify() const { _focusStage.Notify(); }
    void SetFocusStage(Stage* stage);
    bool TrySelectStage(Stage* stage);
    void SetSelectStage(Stage* stage);


    /// <summary>현재 스테이지를 반환합니다.</summary>
    bool IsRemainingStage() const;

    /// <summary>현재 스테이지를 반환합니다.</summary>
    Stage* GetCurrentSelectedStage();

    /// <summary>현재 스테이지의 몬스터 SpawnID를 반환합니다.</summary>
    Monster::SpawnID GetCurrentSpawnID() const;

public:
    REFLECT_PROPERTY(MainBackgroundImage, MapScenePath, BackgroundImage, StageEnableImage, StageDisableImage,
                     StageFocusImage, RewardPopupImage)

    GETTER_ONLY(std::string, MapScenePath) { return ReflectFields->MapSceneGuid; }
    PROPERTY(MapScenePath)

    GETTER(int, MainBackgroundImage) { return ReflectFields->MainBackgroundAssetID; }
    SETTER(int, MainBackgroundImage)
    {
        ReflectFields->MainBackgroundAssetID = value;
        SetMainBackgroundAsset(value);
    }
    PROPERTY(MainBackgroundImage)

    GETTER(int, BackgroundImage) { return ReflectFields->AssetIDs[BACKGROUND]; }
    SETTER(int, BackgroundImage)
    { 
        ReflectFields->AssetIDs[BACKGROUND] = value;
        ChageBackgroundImage(value);
    }
    PROPERTY(BackgroundImage)

    GETTER(int, StageEnableImage) { return ReflectFields->AssetIDs[STAGE_ENABLE]; }
    SETTER(int, StageEnableImage) { ReflectFields->AssetIDs[STAGE_ENABLE] = value; }
    PROPERTY(StageEnableImage)

    GETTER(int, StageDisableImage) { return ReflectFields->AssetIDs[STAGE_DISABLE]; }
    SETTER(int, StageDisableImage) { ReflectFields->AssetIDs[STAGE_DISABLE] = value; }
    PROPERTY(StageDisableImage)

    GETTER(int, StageFocusImage) { return ReflectFields->AssetIDs[STAGE_FOCUS]; }
    SETTER(int, StageFocusImage) { ReflectFields->AssetIDs[STAGE_FOCUS] = value; }
    PROPERTY(StageFocusImage)

    GETTER(int, RewardPopupImage) { return ReflectFields->AssetIDs[REWARD_POPUP]; }
    SETTER(int, RewardPopupImage) { ReflectFields->AssetIDs[REWARD_POPUP] = value; }
    PROPERTY(RewardPopupImage)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::array<int, MAX> AssetIDs;
    std::string          MapSceneGuid; // 자기 자신 씬 Guid (씬 로드 시 비활성화 시키기 위해)
    int                  MainBackgroundAssetID = 0; // 맵 백그라운드 이미지 에셋 ID
    REFLECT_FIELDS_END(MapManager)

protected:
    void ImGuiDrawPropertysEvent() override;

private:
    void ChageBackgroundImage(int assetID);
    void DefaultSetting();
    void RegisterStage(GameObject& object);

    void UpdateStageUI();
    
    // 해당 스테이지가 Submit가능한 상태인지 판단합니다
    bool CanSubmitStage(Stage* stage);

private:
    SingletonObject<MapManager>             _singletonObject{this};
    SingletonComponent<MapManager>          _singletonComponent{this};

    MVVM::Model<Stage*>                     _focusStage;

    std::vector<Stage*>                     _stages;
    std::map<int, std::map<int, Stage*>>    _stageDataTable;

    ScrollingWrapper*                       _scroll = nullptr;
    float                                   _scrollSpeed  = 100.0f;

private:
    Stage*      _selectedStage          = nullptr;
    StageData   _lastClearedStageData   = {};

private:
    void PreferencesKeyDown(const Input::Controller&);
    void InventoryKeyDown(const Input::Controller&);
    void ScrollKeyUpdate(const Input::Controller& controller);
    void OpenPreferencesWindow();
    void OpenInventoryWindow();

    Stage* _lastFocusStage = nullptr;
    bool   _openPreferences = false;
    bool   _openInventory   = false;

    float _scrollDir = 0.f;

private:
    ImageElement* _mainBackgroundUI;
    void          SetMainBackgroundAsset(int assetID);
};