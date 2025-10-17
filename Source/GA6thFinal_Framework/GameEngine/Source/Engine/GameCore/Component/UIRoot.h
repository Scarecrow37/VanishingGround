#pragma once
#include "Engine/GameCore/InputReciver/InputReceiver.h"

class UINavigationComponent;

enum class FocusCallType : unsigned char
{
    INITIAL,
    INPUT,
    FORCED
};

constexpr unsigned int MAX_NAVIGATION_LOOP_COUNT = 100;

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
    void SetInitialFocus(const UINavigationComponent* uiComponent);
    void RequestChangeFocusComponent(UINavigationComponent* nextFocusComponent);
    void CheckNavigationIdFlawless(const UIBaseComponent* newComponent);

    NavigationID           AcquireNavigationID(NavigationID tempID);
    NavigationID           ReleaseNavigationID(NavigationID id);
    UINavigationComponent* FindNavigationComponent(NavigationID id);

protected:
    void ImGuiDrawPropertysEvent() override;
    void Reset() override;
    void Start() override;

private:
    bool ChangeFocusComponent(UINavigationComponent* nextFocusComponent, FocusCallType callType);
    void UpdateNavigation();
    void UpdateNavigationMap();
    void UpdateNavigationMap(Transform& exceptTransform);
    void ChangeNavigationID(NavigationID from, NavigationID to);

    NavigationID           GetSpareID();
    UINavigationComponent* FindNavigationComponentInTransform(NavigationID id) const;

protected:
    REFLECT_FIELDS_BEGIN(UIBaseComponent)
    NavigationID LastID = INVALID_NAVIGATION_ID + 1;
    std::unordered_set<NavigationID> SpareID;
    NavigationID           InitialFocusID = INVALID_NAVIGATION_ID;
    REFLECT_FIELDS_END(UIRoot)

    UINavigationComponent* _currentFocusNavigation;
    std::unordered_map<NavigationID, UINavigationComponent*> _navigationMap;
};