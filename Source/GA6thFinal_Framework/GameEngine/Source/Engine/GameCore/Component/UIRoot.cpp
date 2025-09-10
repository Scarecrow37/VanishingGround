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

void UIRoot::ImGuiDrawPropertysEvent()
{
    UIBaseComponent::ImGuiDrawPropertysEvent();

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