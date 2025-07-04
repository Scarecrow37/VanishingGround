#pragma once

namespace Input
{
    /// <summary>
    /// 입력 예외가 발생했을 때 사용하는 예외 클래스입니다.
    /// </summary>
    class InputException : public std::exception
    {
    public:
        explicit InputException(const std::string& message = "Input exception occurred");
    };

    /// <summary>
    /// 입력 장치가 연결되지 않았을 때 발생하는 예외입니다.
    /// </summary>
    class DeviceNotConnectedException final : public InputException
    {
    public:
        explicit DeviceNotConnectedException(const std::string& message = "Input device is not connected");
    };
} // namespace Input