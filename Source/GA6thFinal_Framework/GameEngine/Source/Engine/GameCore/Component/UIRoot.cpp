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

NavigationID UIRoot::AcquireNavigationID()
{
    if (std::set<NavigationID>& spareID = ReflectFields->SpareID; spareID.empty())
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

void UIRoot::ReleaseNavigationID(const NavigationID id)
{
    if (INVALID_NAVIGATION_ID != id)
    {
        ReflectFields->SpareID.insert(id);
    }
}

void UIRoot::SetInitialFocus(UINavigationComponent* uiComponent)
{
    if (nullptr != uiComponent)
    {
        if (nullptr != _currentFocusNavigation)
        {
            _currentFocusNavigation->ResetInitialFocus();
        }
        ChangeFocusComponent(uiComponent);
    }
}

std::optional<NavigationKey> UIRoot::GetPressedButton()
{
    std::optional<NavigationKey> result = std::nullopt;
    if (nullptr != _controller)
    {
        _controller->UpdateState();
        if (const auto& queue = _controller->GetButtonQueue(); false == queue.empty())
        {
            NavigationKey key;
            key.ButtonType = queue.front();
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

    ImGui::Checkbox("Enable Navigation On Editor", &_isEnabledNavigation);

    if (_isDebug)
    {
        constexpr ImGuiDebug debug;
        const NavigationID id = ReflectFields->LastID;
        debug("Last ID", id);

        for (const std::set<NavigationID>& spareID = ReflectFields->SpareID; const NavigationID spare : spareID)
        {
            debug("Spare ID", spare);
        }
    }
}

void UIRoot::OnDrawDebugOverride()
{
    UIBaseComponent::OnDrawDebugOverride();

    EditorUpdate();
}

void UIRoot::OnDrawDebugSelectedOverride()
{
    UIBaseComponent::OnDrawDebugSelectedOverride();

    EditorUpdate();
}

void UIRoot::EditorUpdate()
{
    if (_isEnabledNavigation)
    {
        _controller->UpdateState();
        UpdateNavigation();
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

void UIRoot::UpdateNavigation()
{
    auto queue = _controller->GetButtonQueue();
    std::ranges::for_each(queue, [this](const Input::Controller::Button button) {
        if (nullptr != _currentFocusNavigation)
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
            ChangeFocusComponent(nextFocus);
        }
    });
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
        _navigationMap.insert({id, component});
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