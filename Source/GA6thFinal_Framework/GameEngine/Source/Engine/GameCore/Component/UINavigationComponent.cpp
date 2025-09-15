#include "pch.h"
#include "UINavigationComponent.h"

NavigationID UINavigationComponent::_toID = INVALID_NAVIGATION_ID;

UINavigationComponent::UINavigationComponent() = default;

void UINavigationComponent::FocusIn()
{
    if (UIComponent* siblingUI = SiblingUI; nullptr != siblingUI)
    {
        siblingUI->IsFocus = true;
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"UI Component를 찾을 수 없습니다.");
    }
}

void UINavigationComponent::FocusOut()
{
    if (UIComponent* siblingUI = SiblingUI; nullptr != siblingUI)
    {
        siblingUI->IsFocus = false;
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"UI Component를 찾을 수 없습니다.");
    }
}

void UINavigationComponent::SetInitialFocus()
{
    ReflectFields->IsInitialFocus = true;
    if (UIRoot* root = Root; nullptr != root)
    {
        root->SetInitialFocus(this);
    }
}

void UINavigationComponent::ResetInitialFocus()
{
    ReflectFields->IsInitialFocus = false;
}

UIComponent* UINavigationComponent::GetSiblingUI() const
{
    const GameObject& object = gameObject;
    return object.GetComponentDynamic<UIComponent>();
}

struct NavigationImGuiStep
{
    NavigationImGuiStep() = default;

    void Reset()
    {
        IsPrepareTo    = false;
        IsWaitingInput = false;
        IsPressedButton = false;
        PressedKey      = NavigationKey();
    }

    void PrepareTo(const bool isPrepareTo)
    {
        IsPrepareTo    = isPrepareTo;
        IsWaitingInput = false;
        IsPressedButton = false;
    }

    void WaitingInput(const bool isWaitingInput)
    {
        IsWaitingInput = isWaitingInput;
        IsPressedButton = false;
    }

    void PressedButton(const NavigationKey& key)
    {
        IsWaitingInput    = false;
        IsPressedButton = true;
        PressedKey        = key;
    }

    bool          IsPrepareTo;
    bool          IsWaitingInput;
    bool          IsPressedButton;
    NavigationKey PressedKey;
};

void UINavigationComponent::ImGuiDrawPropertysEvent()
{
    UIBaseComponent::ImGuiDrawPropertysEvent();

    if (ImGui::Button("Set Initial Focus"))
    {
        SetInitialFocus();
    }

    if (ImGui::Button("Clear Navigation Route"))
    {
        ClearNavigationRoute();
    }


    static NavigationImGuiStep step;
    // TO
    {
        if (ImGui::Button("To"))
        {
            _toID = ReflectFields->NavigationID;
            step.PrepareTo(_toID != INVALID_NAVIGATION_ID);
        }

        if (step.IsPrepareTo)
        {
            ImGui::SameLine();
            ImGui::Text("%d", _toID);
        }
    }

    // Navigation Key
    if (step.IsPrepareTo)
    {
        if (ImGui::Button("Navigation Key"))
        {
            step.WaitingInput(true);
        }

        if (step.IsWaitingInput)
        {
            ImGui::SameLine();
            ImGui::Text("Waiting Input...");

            if (const std::optional<NavigationKey> pressedKey = UIRoot::GetPressedButton(); pressedKey.has_value())
            {
                const NavigationKey key = pressedKey.value();
                step.PressedButton(key);
            }
        }

        if (step.IsPressedButton)
        {
            ImGui::SameLine();
            ImGui::Text(step.PressedKey.Name.c_str());
        }
    }

    // From
    if (step.IsPressedButton && _toID != INVALID_NAVIGATION_ID && ImGui::Button("From"))
    {
        AddNavigationRoute(step.PressedKey, _toID);
        _toID = INVALID_NAVIGATION_ID;
        step.Reset();
    }

    if (_isDebug)
    {
        const NavigationID id = ReflectFields->NavigationID;
        ImGuiDebug()("Navigation ID", id);

        ImGui::Text("Navigation Route");
        const auto&  navigationInfos = ReflectFields->NavigationRoutes;
        for (auto& [button, bias, name, toID] : navigationInfos)
        {
            ImGuiDebug()(name.c_str(), toID);
        }
    }
}

void UINavigationComponent::OnDrawDebugSelectedOverride()
{
    UIBaseComponent::OnDrawDebugSelectedOverride();

    if (const UIComponent* siblingUI = SiblingUI; nullptr != siblingUI)
    {
        POINT start = siblingUI->AbsoluteCenterPoint;
        RECT  rect  = siblingUI->AbsoluteRect;
        if (UIRoot* root  = Root; nullptr != root)
        {
            NavigationRoutes navigationInfos = ReflectFields->NavigationRoutes;
            std::ranges::for_each(navigationInfos, [this, start, rect, root](const auto& info) {
                const auto& [button, bias, name, toID] = info;
                if (const UINavigationComponent* toNavigation = root->FindNavigationComponent(toID); nullptr != toNavigation)
                {
                    if (const UIComponent* toSiblingUI = toNavigation->SiblingUI; nullptr != toSiblingUI)
                    {
                        const POINT end = toSiblingUI->AbsoluteCenterPoint;
                        const RECT  toRect = toSiblingUI->AbsoluteRect;
                        if (auto collisionPoints = DrawDebug()(start, end, rect, toRect); collisionPoints.has_value())
                        {
                            auto& [pointA, pointB] = collisionPoints.value();
                            DrawDebug()(pointA, pointB, 50.0f, Colors::Purple);
                        }
                    }
                }
            });
        }
    }
}

void UINavigationComponent::OnAttachParent(GameObject* childGameObject)
{
    UIBaseComponent::OnAttachParent(childGameObject);

    if (nullptr != childGameObject)
    {
        if (UIRoot* uiRoot = GetRoot(*childGameObject); nullptr != uiRoot)
        {
            AcquireNavigationID(uiRoot);
        }
    }
}

void UINavigationComponent::OnDetachParent(GameObject* previousParentGameObject)
{
    UIBaseComponent::OnDetachParent(previousParentGameObject);

    if (nullptr != previousParentGameObject)
    {
        if (UIRoot* uiRoot = GetRoot(*previousParentGameObject); nullptr != uiRoot)
        {
            ReleaseNavigationID(uiRoot);
        }
    }
}

void UINavigationComponent::DeserializedReflectEvent()
{
    UIBaseComponent::DeserializedReflectEvent();

    if (ReflectFields->NavigationID == INVALID_NAVIGATION_ID)
    {
        if (UIRoot* uiRoot = Root; nullptr != uiRoot)
        {
            AcquireNavigationID(uiRoot);
        }
    }
}

void UINavigationComponent::Reset()
{
    UIBaseComponent::Reset();

    if (const bool isInitialFocus = ReflectFields->IsInitialFocus; true == isInitialFocus)
    {
        if (UIRoot* uiRoot = Root; nullptr != uiRoot)
        {
            uiRoot->SetInitialFocus(this);
        }
    }
}

UIRoot* UINavigationComponent::GetRoot(const GameObject& gameObject)
{
    UIRoot*           uiRoot            = nullptr;
    const GameObject* rootGameObjectPtr = nullptr;

    const Transform& transform = gameObject.transform;
    if (const Transform* rootTransform = transform.Root; nullptr != rootTransform)
    {
        const GameObject& rootGameObject = rootTransform->gameObject;
        rootGameObjectPtr          = &rootGameObject;
    }
    else
    {
        const GameObject& rootGameObject = transform.gameObject;
        rootGameObjectPtr          = &rootGameObject;
    }

    if (nullptr != rootGameObjectPtr)
    {
        uiRoot = rootGameObjectPtr->GetComponent<UIRoot>();
    }
    
    return uiRoot;
}

void UINavigationComponent::AcquireNavigationID(UIRoot* root)
{
    if (nullptr != root)
    {
        const NavigationID newID    = root->AcquireNavigationID();
        ReflectFields->NavigationID = newID;
    }
}

void UINavigationComponent::ReleaseNavigationID(UIRoot* root)
{
    if (nullptr != root)
    {
        if (const NavigationID id = ReflectFields->NavigationID; INVALID_NAVIGATION_ID != id)
        {
            root->ReleaseNavigationID(id);
            ReflectFields->NavigationID = INVALID_NAVIGATION_ID;
        }
    }
}

void UINavigationComponent::ClearNavigationRoute()
{
    NavigationRoutes& navigationInfos = ReflectFields->NavigationRoutes;
    navigationInfos.clear();
}

void UINavigationComponent::AddNavigationRoute(const NavigationKey& key, const NavigationID toID)
{
    NavigationRoutes& navigationRoutes = ReflectFields->NavigationRoutes;
    navigationRoutes.push_back({key.ButtonType, key.Bias, key.Name, toID});
}

NavigationID UINavigationComponent::GetNavigatedId(const NavigationKey& key)
{
    NavigationRoutes& navigationInfos = ReflectFields->NavigationRoutes;
    auto  result          = navigationInfos | std::views::filter([&key](const auto& info) {
                      const auto& [button, bias, name, toID] = info;
                      return button == key.ButtonType && bias == key.Bias;
                  }) |
                  std::views::take(1) | std::views::elements<3>;

    if (false == result.empty())
    {
        return *result.begin();
    }

    return INVALID_NAVIGATION_ID;
}