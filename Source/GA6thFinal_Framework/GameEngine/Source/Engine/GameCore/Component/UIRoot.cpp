#include "pch.h"
#include "UIRoot.h"

#include "UI/Base/DrawUIComponent/DrawUIComponent.h"

Input::Controller* UIRoot::_controller = nullptr;

UIRoot::UIRoot() = default;

void UIRoot::SortViewOrder() const
{
    int startOrder = 0;

    Transform& transform = this->transform;

    Transform::ForeachPostOrder(transform, [&startOrder](const Transform* dfsTransform) {
        const GameObject& gameObject = dfsTransform->gameObject;

        auto components = gameObject.GetComponents<DrawUIComponent>();

        std::ranges::for_each(components,
                              [&startOrder](DrawUIComponent* component) { component->SetViewOrder(startOrder++); });
    });
}

NavigationID UIRoot::AcquireNavigationID(const NavigationID tempID)
{
    const NavigationID newID = GetSpareID();
    if (tempID < INVALID_NAVIGATION_ID)
    {
        UpdateNavigationMap();
        ChangeNavigationID(tempID, newID);
        _navigationMap.erase(tempID);
        return newID;
    }
    return newID;
}

NavigationID UIRoot::ReleaseNavigationID(const NavigationID id)
{
    if (id > INVALID_NAVIGATION_ID)
    {
        const NavigationID tempID = -id;

        ChangeNavigationID(id, tempID);
        _navigationMap.erase(id);

        ReflectFields->SpareID.insert(id);

        return tempID;
    }
    return INVALID_NAVIGATION_ID;
}

void UIRoot::SetInitialFocus(const UINavigationComponent* uiComponent)
{
    if (nullptr != uiComponent)
    {
        ReflectFields->InitialFocusID = uiComponent->ID;
    }
}

std::optional<NavigationKey> UIRoot::GetPressedButton()
{
    static unsigned int currentButton = 0;
    static unsigned int previousButton = 0;

    std::optional<NavigationKey> result = std::nullopt;
    if (nullptr != _controller)
    {
        _controller->UpdateState();
        if (const auto& queue = _controller->GetButtonQueue(); false == queue.empty())
        {
            const Input::Controller::Button button = queue.front();
            currentButton                    = button;

            if (currentButton != previousButton)
            {
                NavigationKey key;
                key.ButtonType = button;
                key.Name       = Input::Controller::GetButtonName(key.ButtonType);

                if (key.ButtonType == Input::Controller::Button::LEFT_THUMB_STICK)
                {
                    key.Bias = _controller->GetLeftStickBias();
                    key.Name += " " + std::string(Input::Controller::GetStickBiasName(key.Bias));
                }
                else if (key.ButtonType == Input::Controller::Button::RIGHT_THUMB_STICK)
                {
                    key.Bias = _controller->GetRightStickBias();
                    key.Name += " " + std::string(Input::Controller::GetStickBiasName(key.Bias));
                }

                result = std::make_optional(key);
            }
        }
        previousButton = currentButton;
        currentButton  = 0;
    }
    return result;
}

void UIRoot::Update()
{
    UIBaseComponent::Update();

    UpdateNavigation();
}

void UIRoot::ImGuiDrawPropertysEvent()
{
    UIBaseComponent::ImGuiDrawPropertysEvent();

    if (_isDebug)
    {
        constexpr ImGuiDebug debug;

        const NavigationID initialFocusID = ReflectFields->InitialFocusID;
        debug("Initial Focus ID", initialFocusID);

        const NavigationID id = ReflectFields->LastID;
        debug("Last ID", id);

        for (const std::unordered_set<NavigationID>& spareID = ReflectFields->SpareID; const NavigationID spare : spareID)
        {
            debug("Spare ID", spare);
        }

        if (const UINavigationComponent* currentFocus = _currentFocusNavigation; nullptr != currentFocus)
        {
            debug("Current Focus ID", currentFocus->ID);
        }
        else
        {
            debug("Current Focus ID", "NULL");
        }

        const size_t navigationCount = _navigationMap.size();
        debug("Navigation Map Count", navigationCount);
    }
}

void UIRoot::Reset()
{
    UIBaseComponent::Reset();

    if (nullptr == _controller)
    {
        _controller = &ESceneManager::Engine::GetInputSystem().GetController();
        try
        {
            _controller->Connect();
        }
        catch (std::exception& exception)
        {
            UmLogger.Log(LogLevel::LEVEL_INFO, exception.what());
        }
    }
}

void UIRoot::Awake()
{
    UIBaseComponent::Awake();

    UpdateNavigationMap();

    const NavigationID     initialFocusID        = ReflectFields->InitialFocusID;
    UINavigationComponent* initialFocusComponent = FindNavigationComponent(initialFocusID);
    ChangeFocusComponent(initialFocusComponent);
}

void UIRoot::UpdateNavigation()
{
    static unsigned int currentButton = 0;
    static unsigned int previousButton = 0;

    auto queue = _controller->GetButtonQueue();
    std::ranges::for_each(queue, [this](const Input::Controller::Button button) {

        if (_controller->IsButtonDown(button))
        {
            currentButton |= button;
        }
        else if (_controller->IsButtonUp(button))
        {
            currentButton &= ~button;
        }

        if (nullptr != _currentFocusNavigation && !(previousButton & button))
        {
            NavigationKey navigationKey;
            navigationKey.ButtonType = button;
            if (button == Input::Controller::Button::LEFT_THUMB_STICK)
            {
                navigationKey.Bias = _controller->GetLeftStickBias();
            }
            else if (button == Input::Controller::Button::RIGHT_THUMB_STICK)
            {
                navigationKey.Bias = _controller->GetRightStickBias();
            }
            const NavigationID navigationID = _currentFocusNavigation->GetNavigatedId(navigationKey);
            UINavigationComponent* nextFocus    = FindNavigationComponent(navigationID);
            if (_currentFocusNavigation == nextFocus)
            {
                _currentFocusNavigation->Submit();
            }
            else
            {
                ChangeFocusComponent(nextFocus);
            }
        }
    });

    previousButton = currentButton;
}

void UIRoot::UpdateNavigationMap()
{
    Transform& rootTransform = this->transform;
    Transform::ForeachBFS(rootTransform, [this](const Transform* transform) {
        const GameObject& gameObject = transform->gameObject;
        if (UINavigationComponent* navigationComponent = gameObject.GetComponentDynamic<UINavigationComponent>();
            nullptr != navigationComponent)
        {
            const NavigationID id = navigationComponent->ID;
            _navigationMap.try_emplace(id, navigationComponent);
        }
    });
}

void UIRoot::ChangeNavigationID(NavigationID from, NavigationID to)
{
    UpdateNavigationMap();
    std::ranges::for_each(_navigationMap, [from, to](auto& idComponentPair) {
        auto& [id, component] = idComponentPair;
        component->ChangeNavigationDestinationID(from, to);
    });
}

NavigationID UIRoot::GetSpareID()
{
    if (std::unordered_set<NavigationID>& spareID = ReflectFields->SpareID; spareID.empty())
    {
        return ReflectFields->LastID++;
    }
    else
    {
        const NavigationID id = *spareID.begin();
        spareID.erase(spareID.begin());
        return id;
    }
}

UINavigationComponent* UIRoot::FindNavigationComponent(NavigationID id)
{
    UINavigationComponent* component = nullptr;
    if (const auto iter = _navigationMap.find(id); iter != _navigationMap.end())
    {
        component = iter->second;
    }
    else
    {
        component = FindNavigationComponentInTransform(id);
        if (nullptr != component)
        {
            _navigationMap.insert({id, component});
        }
    }
    return component;
}

UINavigationComponent* UIRoot::FindNavigationComponentInTransform(NavigationID id) const
{
    UINavigationComponent* component     = nullptr;
    Transform&             rootTransform = this->transform;
    Transform::ForeachBFS(rootTransform, [&component, id](const Transform* transform) {
        if (component == nullptr)
        {
            const GameObject& gameObject = transform->gameObject;
            if (UINavigationComponent* navigationComponent = gameObject.GetComponentDynamic<UINavigationComponent>();
                nullptr != navigationComponent && navigationComponent->ID == id)
            {
                component = navigationComponent;
            }
        }
    });
    return component;
}

void UIRoot::ChangeFocusComponent(UINavigationComponent* nextFocusComponent)
{
    if (nullptr != nextFocusComponent)
    {
        if (nullptr != _currentFocusNavigation)
        {
            _currentFocusNavigation->FocusOut();
        }
        _currentFocusNavigation = nextFocusComponent;
        if (nullptr != _currentFocusNavigation)
        {
            _currentFocusNavigation->FocusIn();
        }
    }
}