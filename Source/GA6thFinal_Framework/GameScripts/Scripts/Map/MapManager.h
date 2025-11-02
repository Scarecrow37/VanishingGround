#pragma once
#include "Monster/Common/MonsterCommon.h"
#include "Utility/SingletonHelper.h"

class OpenInventoryComponent;
class Stage;
class ScrollingWrapper;
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
    void OnLoadScene(Scene& loadScene, LoadSceneMode mode) override;

public:
    void    UINotify() const { _focusStage.Notify(); }
    void    SetFocusStage(Stage* stage);
    bool    TrySelectStage(Stage* stage);
    Stage*  GetCurrentSelectedStage();
    Monster::SpawnID GetCurrentSpawnID();

public:
    REFLECT_PROPERTY(MapScenePath, BackgroundImage, StageEnableImage, StageDisableImage, StageFocusImage, RewardPopupImage)
    
    GETTER_ONLY(std::string, MapScenePath) { return ReflectFields->MapScenePath; }
    PROPERTY(MapScenePath)

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
    std::string          MapScenePath;
    REFLECT_FIELDS_END(MapManager)

protected:
    void ImGuiDrawPropertysEvent() override;

private:
    void ChageBackgroundImage(int assetID);
    void DefaultSetting();
    void SetupStage();
    void RegisterStage(GameObject& object);

private:
    SingletonObject<MapManager>             _singletonObject{this};
    SingletonComponent<MapManager>          _singletonComponent{this};

    MVVM::Model<Stage*>                     _focusStage;

    std::vector<Stage*>                     _stages;
    std::map<int, std::map<int, Stage*>>    _stageDataTable;

    ScrollingWrapper*                       _scroll = nullptr;
    float                                   _scrollSpeed  = 100.0f;

private:
    Stage* _selectedStage = nullptr;
    int    _lastClearedStage = 0;

private:
    void PreferencesKeyDown(const Input::Controller&);
    void InventoryKeyDown(const Input::Controller&);
    void OpenPreferencesWindow();
    void OpenInventoryWindow();

    Stage* _lastFocusStage = nullptr;
    bool   _openPreferences = false;
    bool   _openInventory   = false;
};