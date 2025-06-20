#pragma once

namespace Input
{
    class Controller;

    // 현재는 사용하지 않음. 추후 사용할지 말지 결정 필요
    class System
    {
    public:
        void CreateDevice(Controller* controller);
    };
}