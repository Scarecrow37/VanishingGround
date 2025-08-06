#pragma once

namespace Audio
{
    /// <summary>
    /// Audio 소스 객체를 나타내는 클래스입니다.
    /// Manager에 재생을 요청할 때 사용됩니다.
    /// 객체는 복사될 수 없으며, 이동만 가능합니다.
    /// 원본 파일의 크기에 비례해서 Size가 커질 수 있으므로 적절한 시기에 생성 및 삭제를 권장합니다.
    /// </summary>
    class Source
    {
        friend class System;

    public:
        Source(const Source&)            = delete;
        Source& operator=(const Source&) = delete;
        Source(Source&& other) noexcept;
        Source& operator=(Source&& other) noexcept;
        ~Source();

    protected:
        Source(const WAVEFORMATEXTENSIBLE& format, const XAUDIO2_BUFFER& buffer);

    private:
        WAVEFORMATEXTENSIBLE _format;
        XAUDIO2_BUFFER       _buffer;
    };
} // namespace Audio