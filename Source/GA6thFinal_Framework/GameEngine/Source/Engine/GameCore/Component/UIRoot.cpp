#include "pch.h"
#include "UIRoot.h"
#include "UI/Base/DrawUIComponent/DrawUIComponent.h"

REFLECT_FUNCTION(UIRoot)

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

namespace
{
    struct ButtonStateToNavigationKey
    {
        NavigationKey operator()(const Input::Controller::ButtonState& buttonState) const
        {
            NavigationKey key;
            key.ButtonType = buttonState.Button;
            key.Bias       = buttonState.Bias;
            key.Name       = std::string(Input::Controller::GetButtonName(key.ButtonType));
            if (key.ButtonType == Input::Controller::Button::LEFT_THUMB_STICK)
            {
                key.Name += " " + std::string(Input::Controller::GetStickBiasName(key.Bias));
            }
            else if (key.ButtonType == Input::Controller::Button::RIGHT_THUMB_STICK)
            {
                key.Name += " " + std::string(Input::Controller::GetStickBiasName(key.Bias));
            }
            return key;
        }
    };
}

std::optional<NavigationKey> UIRoot::GetPressedButton()
{
    std::optional<NavigationKey> result = std::nullopt;
    if (nullptr != _controller)
    {
        _controller->UpdateState();
        if (const auto& queue = _controller->GetButtonQueue(); false == queue.empty())
        {
            if (const Input::Controller::ButtonState buttonState = queue.front();
                buttonState.Flag == Input::ControllerTypes::STATE_DOWN)
            {
                NavigationKey key = ButtonStateToNavigationKey()(buttonState);

                result = std::make_optional(key);
            }
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

    if (ImGui::Button("Reset View Order"))
    {
        SortViewOrder();
    }

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

void UIRoot::Start()
{
    UIBaseComponent::Start();

    UpdateNavigationMap();

    const NavigationID     initialFocusID        = ReflectFields->InitialFocusID;
    UINavigationComponent* initialFocusComponent = FindNavigationComponent(initialFocusID);
    ChangeFocusComponent(initialFocusComponent, FocusCallType::INITIAL);

    SortViewOrder();
}

void UIRoot::UpdateNavigation()
{
    auto queue = _controller->GetButtonQueue();
    std::ranges::for_each(queue, [this](const Input::Controller::ButtonState& buttonState) {

        if (nullptr != _currentFocusNavigation && (buttonState.Flag == Input::Controller::StateFlag::STATE_DOWN ||
                                                   buttonState.Flag == Input::Controller::StateFlag::STATE_REPEAT))
        {
            const NavigationKey navigationKey = ButtonStateToNavigationKey()(buttonState);

            UINavigationComponent* currentCheckUINavigationComponent = _currentFocusNavigation;
            for (unsigned int tryCount = 0; tryCount < MAX_NAVIGATION_LOOP_COUNT; ++tryCount)
            {
                const NavigationID navigationID = currentCheckUINavigationComponent->GetNavigatedId(navigationKey);
                if (navigationID == INVALID_NAVIGATION_ID)
                    break;

                UINavigationComponent* nextFocus = FindNavigationComponent(navigationID);
                if (currentCheckUINavigationComponent == nextFocus)
                {
                    currentCheckUINavigationComponent->Submit();
                    break;
                }

                if (ChangeFocusComponent(nextFocus, FocusCallType::INPUT))
                    break;

                currentCheckUINavigationComponent = nextFocus;
            }
        }
    });
}

void UIRoot::UpdateNavigationMap(Transform& exceptTransform)
{
    std::set<Transform*> exceptTransforms;
    exceptTransforms.emplace(&exceptTransform);
    Transform::ForeachBFS(exceptTransform, [&exceptTransforms](Transform* t) { exceptTransforms.emplace(t); });

    Transform& rootTransform = this->transform;
    Transform::ForeachBFS(rootTransform, [this, &exceptTransforms](Transform* transform) {
        if (false == exceptTransforms.contains(transform))
        {
            const GameObject& gameObject = transform->gameObject;
            if (UINavigationComponent* navigationComponent = gameObject.GetComponentDynamic<UINavigationComponent>();
                nullptr != navigationComponent)
            {
                const NavigationID id = navigationComponent->ID;
                if (auto [iter, succeed] = _navigationMap.try_emplace(id, navigationComponent);
                    !succeed && iter->second != navigationComponent)
                {
                    navigationComponent->AcquireNavigationID(this);
                }
            }
        }
    });
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
            if (auto [iter, succeed] = _navigationMap.try_emplace(id, navigationComponent);
                !succeed && iter->second != navigationComponent)
            {
                navigationComponent->AcquireNavigationID(this);
            }
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

NavigationID UIRoot::GetFocusedNavigationID() const
{
    return _currentFocusNavigation != nullptr ? _currentFocusNavigation->ID : INVALID_NAVIGATION_ID;
}

UINavigationComponent* UIRoot::GetFocusedNavigationComponent() const
{
    return _currentFocusNavigation;
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

bool UIRoot::ChangeFocusComponent(UINavigationComponent* nextFocusComponent, FocusCallType callType)
{
    if (nullptr != nextFocusComponent)
    {
        if (const bool isEnable = nextFocusComponent->EnableInHierarchy; true == isEnable)
        {
            if (nullptr != _currentFocusNavigation)
            {
                _currentFocusNavigation->FocusOut(callType);
            }
            _currentFocusNavigation = nextFocusComponent;
            if (nullptr != _currentFocusNavigation)
            {
                _currentFocusNavigation->FocusIn(callType);
            }
            return true;
        }
    }
    return false;
}

void UIRoot::RequestChangeFocusComponent(UINavigationComponent* nextFocusComponent)
{
    ChangeFocusComponent(nextFocusComponent, FocusCallType::FORCED);
}

void UIRoot::CheckNavigationIdFlawless(const UIBaseComponent* newComponent)
{
    if (nullptr != newComponent)
    {
        Transform& transform = newComponent->transform;
        UpdateNavigationMap(transform);

        NavigationID maxID = INVALID_NAVIGATION_ID;

        std::vector<UINavigationComponent*> uiNavigationComponents;

        Transform::ForeachBFS(transform, [this, &uiNavigationComponents, &maxID](const Transform* dfsTransform) {
            const GameObject& gameObject = dfsTransform->gameObject;
            if (UINavigationComponent* navigationComponent = gameObject.GetComponentDynamic<UINavigationComponent>();
                nullptr != navigationComponent)
            {
                uiNavigationComponents.push_back(navigationComponent);
                NavigationID id = navigationComponent->ID;
                maxID           = std::max(maxID, id);
            }
        });

        if (ReflectFields->LastID <= maxID)
        {
            ReflectFields->LastID = maxID + 1;
        }

        std::ranges::for_each(uiNavigationComponents, [this, &uiNavigationComponents](
                                                          UINavigationComponent* navigationComponent) {
            if (const NavigationID prevID = navigationComponent->ID; prevID > INVALID_NAVIGATION_ID)
            {
                if (const auto iter = _navigationMap.find(prevID);
                    iter != _navigationMap.end() && iter->second != navigationComponent)
                {
                    const NavigationID newID = GetSpareID();
                    navigationComponent->SetID(newID);
                    std::ranges::for_each(uiNavigationComponents,
                                          [this, prevID, newID](UINavigationComponent* otherNavigationComponent) {
                                              otherNavigationComponent->ChangeNavigationDestinationID(prevID, newID);
                                          });
                }
            }
        });
    }
}