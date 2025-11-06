#pragma once
#include "UmFramework.h"
#include "ItemDropSystem/Interface/IDropItem.h"
#include "Utility/SingletonHelper.h"

class ArtifactUIManager;
class ItemInfoUIManager;
class WeaponChangeUIManager;
class RestartStageNavi;
class EraseRevelationUIManager;
class ReturnToMapNavi;
class TextElement;
class DescriptionPanel;
class ImageElement;
class ItemDropUIRootManager : public Component, public InputReceiver
{
    USING_PROPERTY(ItemDropUIRootManager)
public:
    inline static constexpr const char* TAG = "Item Drop UI Root";
    ItemDropUIRootManager();
    ~ItemDropUIRootManager() override;

public:
    GETTER_ONLY(ArtifactUIManager*, ArtifactUI) 
    { 
        ArtifactUIManager* artifactUI = nullptr;
        if (auto uiManager = _artifactUIManager.lock())
        {
            artifactUI = uiManager.get();
        }
        return artifactUI;
    }
    // 보상 UI의 Artifact 부분을 관리하는 컴포넌트입니다.
    // type : ArtifactUIManager*
    PROPERTY(ArtifactUI)

    GETTER_ONLY(ItemInfoUIManager*, ItemInfoUI) 
    { 
        ItemInfoUIManager* infoUI = nullptr;
        if (auto uiManager = _itemInfoUIManager.lock())
        {
            infoUI = uiManager.get();
        }
        return infoUI;
    }
    // 보상 UI의 포커스된 아이템 정보를 표시하는 UI를 관리하는 컴포넌트입니다.
    // type : ItemInfoUIManager*
    PROPERTY(ItemInfoUI)

    GETTER_ONLY(WeaponChangeUIManager*, WeaponChangeUI)
    { 
        WeaponChangeUIManager* weaponChangeUI = nullptr;
        if (auto weaponChangeManager = _weaponChangeUIManager.lock())
        {
            weaponChangeUI = weaponChangeManager.get();
        }
        return weaponChangeUI;
    }
    // 무기 교체 UI 관리 컴포넌트입니다.
    // type : WeaponChangeUIManager*
    PROPERTY(WeaponChangeUI)

    GETTER_ONLY(EraseRevelationUIManager*, EraseRevelationUI)
    { 
        EraseRevelationUIManager* eraseRevelationUI = nullptr;
        if (auto eraseRevelationUIManager = _eraseRevelationUIManager.lock())
        {
            eraseRevelationUI = eraseRevelationUIManager.get();
        }
        return eraseRevelationUI;
    }
    // 무기 교체 UI 관리 컴포넌트입니다.
    // type : WeaponChangeUIManager*
    PROPERTY(EraseRevelationUI)

    /// <summary>
    /// 포커스 가능한 Navi로 포커스 설정을 해줍니다.
    /// </summary>
    void AutoFocus(bool checkInputDir = true);

    /// <summary>
    /// 스토리 정보를 갱신합니다.
    /// </summary>
    void UpdateStory();

public:
    REFLECT_PROPERTY(
    )

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(ItemDropUIRootManager)

    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

    void Reset() override;
    void Awake() override;
    void Start() override;
    void Update() override;
    void LateUpdate() override;

    void UpdateAutoFocus();

    void OnDpadLeft(const Input::Controller&);
    void OnDpadRight(const Input::Controller&);
    void OnDpadUp(const Input::Controller&);
    void OnDpadDown(const Input::Controller&);
    void OnLeftTumbStickDown(const Input::Controller& controller);

private:
    SingletonComponent<ItemDropUIRootManager> _singletonComponent{this};
    std::weak_ptr<ArtifactUIManager>          _artifactUIManager;
    std::weak_ptr<ItemInfoUIManager>          _itemInfoUIManager;
    std::weak_ptr<WeaponChangeUIManager>      _weaponChangeUIManager;
    std::weak_ptr<EraseRevelationUIManager>   _eraseRevelationUIManager;
    std::weak_ptr<RestartStageNavi>           _restartNavi;
    std::weak_ptr<ReturnToMapNavi>            _returnToMapNavi;

    struct StoryPanel
    {
        std::weak_ptr<TextElement>      Title;
        std::weak_ptr<DescriptionPanel> Description;
        std::weak_ptr<ImageElement>     Image;
    }
    _storyPanel;

    enum class InputDir
    {
        IDLE,
        LEFT,
        RIGHT,
        UP,
        DOWN
    }
    _lastInputDir; //마지막 입력 추적용

    bool _isFocusInput        = false;
    bool _isFocusArtifactNavi = false;
};
