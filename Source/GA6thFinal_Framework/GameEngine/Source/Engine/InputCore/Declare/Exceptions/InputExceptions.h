#pragma once

namespace Input
{
    struct ThrowIfFailed
    {
        void operator()(long result, const std::string& message) const;
    };

    class InputException : public std::exception
    {
    public:
        explicit InputException(const std::string& message = "Input exception occurred");
    };

    class DeviceNotConnectedException final : public InputException
    {
    public:
        explicit DeviceNotConnectedException(const std::string& message = "Input device is not connected");
    };
} // namespace Input