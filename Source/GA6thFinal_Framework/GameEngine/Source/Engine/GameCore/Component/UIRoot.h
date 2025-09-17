#pragma once
#include "Engine/GameCore/InputReciver/InputReceiver.h"

class UINavigationComponent;

class UIRoot : public UIBaseComponent
{
    USING_PROPERTY(UIRoot)

public:
    static std::optional<NavigationKey> GetPressedButton();

private:
    static Input::Controller* _controller;


public:
    UIRoot();

public:
    void Update() override;

    void SortViewOrder() const;
    void ReleaseNavigationID(NavigationID id);
    void SetInitialFocus(const UINavigationComponent* uiComponent);
    void ChangeFocusComponent(UINavigationComponent* nextFocusComponent);

    NavigationID           AcquireNavigationID();
    UINavigationComponent* FindNavigationComponent(NavigationID id);

protected:
    void ImGuiDrawPropertysEvent() override;
    void Reset() override;
    void Awake() override;

private:
    void UpdateNavigation();

    UINavigationComponent* FindNavigationComponentInTransform(NavigationID id) const;

protected:
    REFLECT_FIELDS_BEGIN(UIBaseComponent)
    NavigationID LastID = 0;
    std::set<NavigationID> SpareID;
    NavigationID           InitialFocusID = INVALID_NAVIGATION_ID;
    REFLECT_FIELDS_END(UIRoot)

    UINavigationComponent* _currentFocusNavigation;
    std::unordered_map<NavigationID, UINavigationComponent*> _navigationMap;
};