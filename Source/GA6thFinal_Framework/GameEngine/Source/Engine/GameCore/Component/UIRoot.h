#pragma once
#include "Engine/GameCore/InputReciver/InputReceiver.h"

class UINavigationComponent;

class UIRoot : public UIBaseComponent
{
    USING_PROPERTY(UIRoot)

    static Input::Controller* _controller;

public:
    UIRoot();

public:
    void SortViewOrder() const;

    NavigationID AcquireNavigationID();
    void         ReleaseNavigationID(NavigationID id);
    void         SetInitialFocus(UINavigationComponent* uiComponent);

    static std::optional<NavigationKey> GetPressedButton();

    void Update() override;

    UINavigationComponent* FindNavigationComponent(NavigationID id);
protected:
    void ImGuiDrawPropertysEvent() override;
    void OnDrawDebugOverride() override;
    void OnDrawDebugSelectedOverride() override;

    void EditorUpdate();

    void Reset() override;

private:
    void UpdateNavigation();
    UINavigationComponent* FindNavigationComponentInTransform(NavigationID id) const;
    void                   ChangeFocusComponent(UINavigationComponent* nextFocusComponent);

protected:
    REFLECT_FIELDS_BEGIN(UIBaseComponent)
    NavigationID LastID = 0;
    std::set<NavigationID> SpareID;
    REFLECT_FIELDS_END(UIRoot)

    UINavigationComponent* _currentFocusNavigation;
    std::unordered_map<NavigationID, UINavigationComponent*> _navigationMap;

    bool _isEnabledNavigation;
};