#include "pch.h"
#include "UINavigationComponent.h"

NavigationID UINavigationComponent::_toID = INVALID_NAVIGATION_ID;

UINavigationComponent::UINavigationComponent() = default;

void UINavigationComponent::OnFocusIn()
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

void UINavigationComponent::OnFocusOut()
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
    if (step.IsPrepareTo && _toID != ReflectFields->NavigationID)
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

    if (step.IsPressedButton && _toID != INVALID_NAVIGATION_ID && _toID != ReflectFields->NavigationID &&
        ImGui::Button("From"))
    {

        step.Reset();
    }

    if (_isDebug)
    {
        const NavigationID id = ReflectFields->NavigationID;
        ImGuiDebug()("Navigation ID", id);

        if (ImGui::Button("Focus In"))
        {
            OnFocusIn();
        }

        if (ImGui::Button("Focus Out"))
        {
            OnFocusOut();
        }
    }
}

void UINavigationComponent::OnDrawDebugSelectedOverride()
{
    UIBaseComponent::OnDrawDebugSelectedOverride();

    // TODO Draw Navigation Route
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
    //auto& buttons = ReflectFields->NavigationButtons;
    //auto& biases  = ReflectFields->NavigationStickBiases;
    //auto& toIDs   = ReflectFields->NavigationTo;
    //buttons.clear();
    //biases.clear();
    //toIDs.clear();
}

void UINavigationComponent::AddNavigationRoute(const NavigationKey& key, const NavigationID toID)
{
    //auto& buttons = ReflectFields->NavigationButtons;
    //auto& biases  = ReflectFields->NavigationStickBiases;
    //auto& toIDs   = ReflectFields->NavigationTo;
    //buttons.push_back(key.ButtonType);
    //biases.push_back(key.Bias);
    //toIDs.push_back(toID);
}

void UINavigationComponent::GetNavigatedId(const NavigationKey& key)
{
    //auto& buttons = ReflectFields->NavigationButtons;
    //auto& biases  = ReflectFields->NavigationStickBiases;
    //auto& toIDs   = ReflectFields->NavigationTo;
}