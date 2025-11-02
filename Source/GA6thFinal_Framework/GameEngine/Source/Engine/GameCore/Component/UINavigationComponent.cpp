#include "pch.h"
#include "UINavigationComponent.h"

REFLECT_FUNCTION(UINavigationComponent)

NavigationID UINavigationComponent::_toID = INVALID_NAVIGATION_ID;

UINavigationComponent::UINavigationComponent() = default;

void UINavigationComponent::Focus()
{
    if (UIRoot* root = Root; nullptr != root)
    {
        root->RequestChangeFocusComponent(this);
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"UI Root를 찾을 수 없습니다.");
    }
}

void UINavigationComponent::FocusIn(FocusCallType callType)
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

void UINavigationComponent::FocusOut(FocusCallType callType)
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

void UINavigationComponent::SetInitialFocus() const
{
    if (UIRoot* root = Root; nullptr != root)
    {
        root->SetInitialFocus(this);
    }
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

namespace
{
    struct EraseLater
    {
        using Container = UINavigationComponent::NavigationRoutes;
        using Element   = UINavigationComponent::NavigationRoute;
        explicit EraseLater(Container* map) : ContainerPointer(map) {}
        EraseLater(const EraseLater&)                = delete;
        EraseLater& operator=(const EraseLater&)     = delete;
        EraseLater(EraseLater&&) noexcept            = delete;
        EraseLater& operator=(EraseLater&&) noexcept = delete;
        ~EraseLater()
        {
            std::erase_if(*ContainerPointer,
                          [this](const Element& element) { return ElementsToErase.contains(element); });
            ElementsToErase.clear();
        }

        void operator()(const Element& element) { ElementsToErase.emplace(element); }

        Container*        ContainerPointer;
        std::set<Element> ElementsToErase;
    };


    struct Row
    {
        void operator()(const std::string& name, int id, const std::function<void()>& deleteCallback) const
        {
            ImGui::TableSetColumnIndex(0);
            ImVec2 availSize = ImGui::GetContentRegionAvail();
            ImGui::SetNextItemWidth(availSize.x);
            ImGui::Selectable(name.c_str());

            ImGui::TableSetColumnIndex(1);
            availSize = ImGui::GetContentRegionAvail();
            ImGui::SetNextItemWidth(availSize.x - 60.f);
            ImGui::BeginDisabled();
            ImGui::InputInt("##id", &id, 0);
            ImGui::EndDisabled();

            const float height = ImGui::GetItemRectSize().y;
            ImGui::SameLine();
            if (ImGui::Button("-", ImVec2(height, height)))
            {
                deleteCallback();
            }
        }
    };
} // namespace

void UINavigationComponent::ImGuiDrawPropertysEvent()
{
    UIBaseComponent::ImGuiDrawPropertysEvent();

    const NavigationID id = ReflectFields->NavigationID;
    ImGuiDebug()("Navigation ID", id);

    if (ImGui::Button("Set Initial Focus"))
    {
        SetInitialFocus();
    }

    if (ImGui::TreeNodeEx("Navigation Route"))
    {
        if (ImGui::BeginTable("NavigationRouteTable##Detail", 2, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthStretch, 0.6f);
            ImGui::TableSetupColumn("Destination ID", ImGuiTableColumnFlags_WidthStretch, 0.4f);
            ImGui::TableHeadersRow();

            NavigationRoutes& navigationRoutes = ReflectFields->NavigationRoutes;
            EraseLater        eraseLater(&navigationRoutes);

            // Existing Routes
            std::ranges::for_each(navigationRoutes, [this, &eraseLater](const NavigationRoute& route) {
                auto& [button, bias, name, toID] = route;
                ImGui::PushID(name.c_str());
                ImGui::TableNextRow();
                Row()(name, toID, [&eraseLater, &route]() {
                    eraseLater(route);
                });

                ImGui::PopID();
            });

            // New Key
            static bool           isSelected     = false;
            constexpr const char* defaultMessage = "Click to receive input.";
            static std::string    keyInput       = defaultMessage;
            static NavigationID   idInput        = INVALID_NAVIGATION_ID;
            static NavigationKey  lastKey;

            if (isSelected)
            {
                if (const std::optional<NavigationKey> pressedKey = UIRoot::GetPressedButton(); pressedKey.has_value())
                {
                    lastKey = pressedKey.value();
                    keyInput = lastKey.Name;
                    isSelected = false;
                }
            }

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImVec2 availSize = ImGui::GetContentRegionAvail();
            ImGui::SetNextItemWidth(availSize.x);
            ImGui::Selectable(keyInput.c_str(), &isSelected);

            ImGui::TableSetColumnIndex(1);
            availSize = ImGui::GetContentRegionAvail();
            ImGui::SetNextItemWidth(availSize.x - 60.f);
            ImGui::InputInt("##id", &idInput, 0);

            const float height = ImGui::GetItemRectSize().y;
            ImGui::SameLine();
            if (false == lastKey.Name.empty() && idInput != INVALID_NAVIGATION_ID && ImGui::Button("+", ImVec2(height, height)))
            {
                AddNavigationRoute(lastKey, idInput);
                isSelected = false;
                keyInput   = defaultMessage;
                idInput    = INVALID_NAVIGATION_ID;
                lastKey    = NavigationKey();
            }

            ImGui::EndTable();
        }

        if (ImGui::Button("Clear Navigation Route"))
        {
            ClearNavigationRoute();
        }

        ImGui::TreePop();
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

void UINavigationComponent::OnAttachParent(GameObject* parentGameObject)
{
    UIBaseComponent::OnAttachParent(parentGameObject);

    if (nullptr != parentGameObject)
    {
        if (UIRoot* uiRoot = GetRoot(*parentGameObject); nullptr != uiRoot)
        {
            uiRoot->CheckNavigationIdFlawless(this);
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

    if (const NavigationID id = ReflectFields->NavigationID; id == INVALID_NAVIGATION_ID)
    {
        if (UIRoot* uiRoot = Root; nullptr != uiRoot)
        {
            AcquireNavigationID(uiRoot);
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
        const NavigationID tempID = ReflectFields->NavigationID;
        const NavigationID newID    = root->AcquireNavigationID(tempID);
        ReflectFields->NavigationID  = newID;
    }
}

void UINavigationComponent::ShowTooltips() {}

void UINavigationComponent::HideTooltips() {}

void UINavigationComponent::ReleaseNavigationID(UIRoot* root)
{
    if (nullptr != root)
    {
        if (const NavigationID id = ReflectFields->NavigationID; INVALID_NAVIGATION_ID != id)
        {
            const NavigationID tempId = root->ReleaseNavigationID(id);
            ReflectFields->NavigationID = tempId;
        }
    }
}

void UINavigationComponent::SetID(const NavigationID id)
{
    ReflectFields->NavigationID = id;
}

void UINavigationComponent::ClearNavigationRoute()
{
    NavigationRoutes& navigationRoutes = ReflectFields->NavigationRoutes;
    navigationRoutes.clear();
}

void UINavigationComponent::ChangeNavigationDestinationID(NavigationID fromId, NavigationID toId)
{
    NavigationRoutes& navigationRoutes = ReflectFields->NavigationRoutes;
    std::ranges::for_each(navigationRoutes, [fromId, toId](NavigationRoute& info) {
        if (auto& [button, bias, name, toID] = info; toID == fromId)
        {
            toID = toId;
        }
    });
}

void UINavigationComponent::AddNavigationRoute(const NavigationKey& key, const NavigationID toID)
{
    NavigationRoutes& navigationRoutes = ReflectFields->NavigationRoutes;
    navigationRoutes.push_back(std::make_tuple(key.ButtonType, key.Bias, key.Name, toID));
}

void UINavigationComponent::RemoveNavigationRoute(const NavigationKey& key)
{
    NavigationRoutes& navigationInfos = ReflectFields->NavigationRoutes;
    std::erase_if(navigationInfos, [&key](const auto& info) {
        const auto& [button, bias, name, toID] = info;
        return button == key.ButtonType && bias == key.Bias;
    });
}

NavigationID UINavigationComponent::GetNavigatedId(const NavigationKey& key)
{
    NavigationRoutes& navigationInfos = ReflectFields->NavigationRoutes;
    auto              result          = navigationInfos | std::views::filter([&key](const auto& info) {
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