#pragma once

/// <summary>
/// Input을 사용할 컴포넌트에 상속해 사용합니다. 
/// </summary>
class InputReceiver
{
public:
    using Action = ESceneManager::InputSystem::Action;
    using ControllerButton = ESceneManager::InputSystem::ControllerButton;

    InputReceiver() = default;
    virtual ~InputReceiver();

    template <typename T>
    bool BindInputAction(ControllerButton button, Action action, T* instance, void (T::*func)(const Input::Controller&),
                    std::source_location = std::source_location::current());

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
                inputSystem._receivers[buttonIndex][actionIndex].emplace_back(
                    instance, 
                    [instance, func](const Input::Controller& controller) 
                    {
                        if (instance->EnableInHierarchy)
                        {
                            std::invoke(func, instance, controller);
                        }
                    }
                );
            }  
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"이미 바인딩된 액션입니다.", location);
        }
    }
    return result;
}
