#pragma once

namespace Audio
{
    class Source
    {
        friend class EManager;

    public:
        Source(const WAVEFORMATEX& format, const XAUDIO2_BUFFER& buffer);
        Source(const Source&)            = delete;
        Source& operator=(const Source&) = delete;
        Source(Source&& other) noexcept;
        Source& operator=(Source&& other) noexcept;
        ~Source();

    private:
        WAVEFORMATEX _format;
        XAUDIO2_BUFFER       _buffer;
    };
} // namespace Audio