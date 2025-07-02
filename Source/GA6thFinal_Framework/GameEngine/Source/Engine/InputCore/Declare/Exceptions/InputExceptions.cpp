#include "pch.h"
#include "InputExceptions.h"

namespace Input
{
    void ThrowIfFailed::operator()(const DWORD result, const std::string& message) const
    {
        if (result != ERROR_SUCCESS)
        {
            if (result == ERROR_DEVICE_NOT_CONNECTED)
                throw DeviceNotConnectedException(message);
            throw InputException(message);
        }
    }

    InputException::InputException(const std::string& message) : std::exception(message.c_str()) {}

    DeviceNotConnectedException::DeviceNotConnectedException(const std::string& message) : InputException(message) {}
} // namespace Input