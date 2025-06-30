#pragma once

/// <summary>
/// Input을 사용할 컴포넌트에 상속해 사용합니다. 
/// </summary>
class InputReceiver
{
public:
    InputReceiver();
    virtual ~InputReceiver();

    virtual void OnInput(Input::Controller* controller) = 0;
};