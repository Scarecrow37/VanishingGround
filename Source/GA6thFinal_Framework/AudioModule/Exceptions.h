#pragma once

namespace Audio
{
    /// <summary>
    /// 오디오 관련 예외를 나타내는 예외 클래스입니다.
    /// </summary>
    class AudioException : public std::exception
    {
    public:
        explicit AudioException(const std::string& message = "Audio exception occurred");
    };

    /// <summary>
    /// 오디오 작업에서 잘못된 호출이 발생했을 때 던져지는 예외 클래스입니다.
    /// </summary>
    class InvalidCallException final : public AudioException
    {
    public:
        explicit InvalidCallException(const std::string& message = "Invalid call in audio operation");
    };

    /// <summary>
    /// 오디오 디코더에서 오류가 발생했을 때 던져지는 예외 클래스입니다.
    /// </summary>
    class XmaDecoderException final : public AudioException
    {
    public:
        explicit XmaDecoderException(const std::string& message = "XMA decoder error occurred");
    };

    /// <summary>
    /// 오디오 효과 생성에 실패했을 때 던져지는 예외 클래스입니다.
    /// </summary>
    class EffectCreationException final : public AudioException
    {
    public:
        explicit EffectCreationException(const std::string& message = "Effect creation failed");
    };

    /// <summary>
    /// 오디오 장치가 무효화되었을 때 발생하는 예외를 나타냅니다.
    /// </summary>
    class DeviceInvalidatedException final : public AudioException
    {
    public:
        explicit DeviceInvalidatedException(const std::string& message = "Audio device has been invalidated");
    };

    /// <summary>
    /// 잘못된 오디오 핸들에 대한 예외를 나타내는 클래스입니다.
    /// </summary>
    class InvalidHandleException final : public AudioException
    {
    public:
        explicit InvalidHandleException(const std::string& message = "Invalid audio handle");
    };
} // namespace Audio