#pragma once
#include "Utility/SingletonHelper.h"

class Stage;
class ScrollingWrapper;
class MapManager : public Component, public InputReceiver
{
    enum AssetIDs { BACKGROUND, STAGE_ENABLE, STAGE_DISABLE, STAGE_FOCUS, REWARD_POPUP, MAX };
    USING_PROPERTY(MapManager)

public:
    MapManager();
    ~MapManager() override;

public:
    void SetFocusStage(Stage* stage);

public:
    void Awake() override;
    void Update() override;
    void OnLoadScene(Scene& loadScene, LoadSceneMode mode) override;

    void UINotify() const { _focusStage.Notify(); }

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

private:
    ScrollingWrapper* _scroll = nullptr;

private:
    MVVM::Model<Stage*> _focusStage;
    std::vector<Stage*> _stages;
    int                 _childCount   = 0;
    float               _scrollSpeed  = 100.0f;
    int                 _clearedStage = 0;

private:
    SingletonObject<MapManager>    _singletonObject{this};
    SingletonComponent<MapManager> _singletonComponent{this};

    Stage* _lastFocusStage = nullptr;
    void PreferencesKeyDown(const Input::Controller&);
    void OpenPreferencesWindow();

};