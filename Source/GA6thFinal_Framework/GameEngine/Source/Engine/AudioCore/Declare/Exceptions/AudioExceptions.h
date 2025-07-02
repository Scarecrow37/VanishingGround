#pragma once

namespace Audio
{
    struct ThrowIfFailed
    {
        void operator()(HRESULT result, const std::string& message) const;
    };

    class AudioException : public std::exception
    {
    public:
        explicit AudioException(const std::string& message = "Audio exception occurred");
    };

    class InvalidCallException final : public AudioException
    {
    public:
        explicit InvalidCallException(const std::string& message = "Invalid call in audio operation");
    };

    class XmaDecoderException final : public AudioException
    {
    public:
        explicit XmaDecoderException(const std::string& message = "XMA decoder error occurred");
    };

    class EffectCreationException final : public AudioException
    {
    public:
        explicit EffectCreationException(const std::string& message = "Effect creation failed");
    };

    class DeviceInvalidatedException final : public AudioException
    {
    public:
        explicit DeviceInvalidatedException(const std::string& message = "Audio device has been invalidated");
    };

    class InvalidHandleException final : public AudioException
    {
    public:
        explicit InvalidHandleException(const std::string& message = "Invalid audio handle");
    };
} // namespace Audio