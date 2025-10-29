#pragma once

/// <summary>
/// Input을 사용할 컴포넌트에 상속해 사용합니다. 
/// </summary>
class InputReceiver
{
public:
    friend class ESceneManager::InputSystem;
    using Action = ESceneManager::InputSystem::Action;
    using ControllerButton = ESceneManager::InputSystem::ControllerButton;

    InputReceiver() = default;
    virtual ~InputReceiver();

    /// <summary>
    /// 인풋 콜백을 등록합니다.
    /// </summary>
    /// <param name="button :">버튼</param>
    /// <param name="action :">액션</param>
    /// <param name="instance :">this</param>
    /// <param name="func :">callback</param>
    /// <returns>성공 여부</returns>
    template <typename T>
    bool BindInputAction(ControllerButton button, Action action, T* instance, void (T::*func)(const Input::Controller&),
                    std::source_location = std::source_location::current());

    /// <summary>
    /// 인풋 콜백을 등록합니다. 컴포넌트가 아닌 클래스에 상속받을때 사용합니다.
    /// </summary>
    /// <param name="button :">버튼</param>
    /// <param name="action :">액션</param>
    /// <param name="owner :">InputReceiver의 Owner</param>
    /// <param name="instance :">this</param>
    /// <param name="func :">callback</param>
    /// <returns>성공 여부</returns>
    template <typename T>
    bool BindInputAction(ControllerButton button, Action action, Component* owner, T* instance, void (T::*func)(const Input::Controller&),
                    std::source_location = std::source_location::current());

    /// <summary>
    /// 해당 액션에 대해 모든 키 이벤트를 등록합니다.
    /// </summary>
    /// <param name="action :">액션</param>
    /// <param name="owner :">InputReceiver의 Owner</param>
    /// <param name="instance :">this</param>
    /// <param name="func :">callback</param>
    /// <returns>성공 여부</returns>
    template <typename T>
    bool BindAllKeyInputAction(Action action, Component* owner, T* instance, void (T::*func)(const Input::Controller&),
                         std::source_location = std::source_location::current());

    /// <summary>
    /// Input Layer를 Push합니다. Push된 UI만 Input 함수를 Callback 합니다.
    /// </summary>
    /// <returns>성공 여부</returns>
    bool PushInputLayer();

    /// <summary>
    /// Input Layer를 Pop합니다. 소멸자에서 자동으로 한번 호출합니다. 자신이 최상단 레이어가 아니면 실패합니다.
    /// </summary>
    /// <returns>성공 여부</returns>
    bool PopInputLayer();

    static void Vibrate(Input::ControllerTypes::Vibration vibration);

private:
    struct ControllerSetKey
    {
        ESceneManager::InputSystem::Action Action;
        ESceneManager::InputSystem::ControllerButton Button;

        bool operator<(const ControllerSetKey& other) const
        {
            if (Action != other.Action)
                return Action < other.Action;
            return Button < other.Button;
        }
    };
    std::set<ControllerSetKey> _controllerSet;
    std::shared_ptr<bool>      _isDestroy;
    bool                       _isPushStack;
};

/// <summary>
/// <para> Input 이벤트를 바인딩합니다.                                                      </para>
/// <para> 바인딩할 함수는 매개변수로 (const Input::Controller&amp;)를 반드시 전달받아야 합니다.    </para>
/// <para> *마지막 인자는 기본값을 사용해야 로그가 정상적으로 남겨집니다.                       </para>
/// </summary>
/// <param name="button :">사용할 버튼입니다.</param>
/// <param name="action :">사용할 액션입니다.</param>
/// <param name="instance :">컴포넌트의 this 포인터</param>
/// <param name="func :">바인딩할 맴버 함수</param>
/// <returns></returns>
template <typename T>
inline bool InputReceiver::BindInputAction(ControllerButton button, Action action, T* instance,
                                      void (T::*func)(const Input::Controller&), std::source_location location)
{
    static_assert(std::is_base_of_v<Component, T>, "T must be derived from Component.");
    static_assert(std::is_base_of_v<InputReceiver, T>, "T must be derived from InputReceiver.");
    bool result = false;
    if (instance->gameObject->IsValid())
    {
        ControllerSetKey key{};
        key.Button          = button;
        key.Action          = action;
        auto [iter, isInsert] = _controllerSet.insert(key);
        result = isInsert;
        if (result)
        {
            auto& inputSystem = ESceneManager::Engine::GetInputSystem();
            int   buttonIndex = (int)button;
            int   actionIndex = (int)action;
            if (instance->gameObject->IsValid())
            {
                inputSystem.RegisterInputReceiver(*this, buttonIndex, actionIndex,
                [instance, func](const Input::Controller& controller) 
                {
                    if (instance->EnableInHierarchy)
                    {
                        std::invoke(func, instance, controller);
                    }
                });
            }  
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"이미 바인딩된 액션입니다.", location);
        }
    }
    return result;
}

/// <summary>
/// <para> Input 이벤트를 바인딩합니다.                                                          </para>
/// <para> 바인딩할 함수는 매개변수로 (const Input::Controller&amp;)를 반드시 전달받아야 합니다.   </para>
/// <para> *마지막 인자는 기본값을 사용해야 로그가 정상적으로 남겨집니다.                           </para>
/// </summary>
/// <param name="button :">사용할 버튼입니다.</param>
/// <param name="action :">사용할 액션입니다.</param>
/// <param name="owner :">InputReceiver의 생명주기를 관리하는 컴포넌트를 전달해야 합니다.</param>
/// <param name="instance :">InputReciver의 this 포인터</param>
/// <param name="func :">바인딩할 맴버 함수</param>
/// <returns></returns>
template <typename T>
inline bool InputReceiver::BindInputAction(ControllerButton button, Action action, Component* owner, T* instance,
                                           void (T::*func)(const Input::Controller&), std::source_location location)
{
    static_assert(std::is_base_of_v<InputReceiver, T>, "T must be derived from InputReceiver.");
    bool result = false;
    if (owner->gameObject->IsValid())
    {
        ControllerSetKey key{};
        key.Button            = button;
        key.Action            = action;
        auto [iter, isInsert] = _controllerSet.insert(key);
        result                = isInsert;
        if (result)
        {
            auto& inputSystem = ESceneManager::Engine::GetInputSystem();
            int   buttonIndex = (int)button;
            int   actionIndex = (int)action;
            if (owner->gameObject->IsValid())
            {
                inputSystem.RegisterInputReceiver(*this, buttonIndex, actionIndex,
                [instance, owner, func](const Input::Controller& controller) 
                {
                    if (owner->EnableInHierarchy)
                    {
                        std::invoke(func, instance, controller);
                    }
                });
            }
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"이미 바인딩된 액션입니다.", location);
        }
    }
    return result;
}

template <typename T>
inline bool InputReceiver::BindAllKeyInputAction(Action action, Component* owner, T* instance,
                                                 void (T::*func)(const Input::Controller&), std::source_location location)
{
    static_assert(std::is_base_of_v<InputReceiver, T>, "T must be derived from InputReceiver.");
    bool result = false;
    if (owner->gameObject->IsValid())
    {
        constexpr auto enumrators = rfl::get_enumerator_array<ControllerButton>();
        for (auto& [str, button] : enumrators)
        {
            ControllerSetKey key{};
            key.Button = button;
            key.Action = action;
            auto [iter, isInsert] = _controllerSet.insert(key);
            result = isInsert;
            if (result)
            {
                auto& inputSystem = ESceneManager::Engine::GetInputSystem();
                int   buttonIndex = (int)button;
                int   actionIndex = (int)action;
                if (owner->gameObject->IsValid())
                {
                    inputSystem.RegisterInputReceiver(*this, buttonIndex, actionIndex,
                    [instance, owner, func](const Input::Controller& controller) 
                    {
                        if (owner->EnableInHierarchy)
                        {
                            std::invoke(func, instance, controller);
                        }
                    });
                }
            }
            else
            {
                UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"이미 바인딩된 액션입니다.", location);
            }
        }   
    }
    return result;
}
