#include "pch.h"
#include "Exceptions.h"

namespace Input
{
    InputException::InputException(const std::string& message) : std::exception(message.c_str()) {}

    DeviceNotConnectedException::DeviceNotConnectedException(const std::string& message) : InputException(message) {}
} // namespace Input