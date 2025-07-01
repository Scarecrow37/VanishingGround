#include "pch.h"
#include "AudioExceptions.h"

namespace Audio
{
    void ThrowIfFailed::operator()(const HRESULT result, const std::string& message) const
    {
        if (FAILED(result))
        {
            switch (result)
            {
            case XAUDIO2_E_INVALID_CALL:
                throw InvalidCallException(message);
            case XAUDIO2_E_XMA_DECODER_ERROR:
                throw XmaDecoderException(message);
            case XAUDIO2_E_XAPO_CREATION_FAILED:
                throw EffectCreationException(message);
            case XAUDIO2_E_DEVICE_INVALIDATED:
                throw DeviceInvalidatedException(message);
            default:
                LPSTR       buffer;
                const DWORD count       = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                                             FORMAT_MESSAGE_IGNORE_INSERTS,
                                                         nullptr, result, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                                         reinterpret_cast<LPSTR>(&buffer), 0, nullptr);
                std::string fullMessage = message;
                if (buffer != nullptr)
                {
                    fullMessage.append(buffer);
                    LocalFree(buffer);
                }
                throw AudioException(fullMessage);
            }
        }
    }

    AudioException::AudioException(const std::string& message) : std::exception(message.c_str()) {}

    InvalidCallException::InvalidCallException(const std::string& message) : AudioException(message) {}

    XmaDecoderException::XmaDecoderException(const std::string& message) : AudioException(message) {}

    EffectCreationException::EffectCreationException(const std::string& message) : AudioException(message) {}

    DeviceInvalidatedException::DeviceInvalidatedException(const std::string& message) : AudioException(message) {}

    InvalidHandleException::InvalidHandleException(const std::string& message) : AudioException(message) {}
} // namespace Audio