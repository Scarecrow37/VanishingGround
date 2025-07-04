#include "pch.h"
#include "Exceptions.h"

namespace Audio
{
    AudioException::AudioException(const std::string& message) : std::exception(message.c_str()) {}

    InvalidCallException::InvalidCallException(const std::string& message) : AudioException(message) {}

    XmaDecoderException::XmaDecoderException(const std::string& message) : AudioException(message) {}

    EffectCreationException::EffectCreationException(const std::string& message) : AudioException(message) {}

    DeviceInvalidatedException::DeviceInvalidatedException(const std::string& message) : AudioException(message) {}

    InvalidHandleException::InvalidHandleException(const std::string& message) : AudioException(message) {}
} // namespace Audio