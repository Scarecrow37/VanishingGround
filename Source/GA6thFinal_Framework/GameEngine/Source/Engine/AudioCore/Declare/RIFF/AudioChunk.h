#pragma once

namespace Audio
{
    enum Fourcc : FOURCC
    {
#ifdef _XBOX
        RIFF = MAKEFOURCC('F', 'F', 'I', 'R'),
        DATA = MAKEFOURCC('a', 't', 'a', 'd'),
        FMT  = MAKEFOURCC(' ', 't', 'm', 'f'),
        WAVE = MAKEFOURCC('E', 'V', 'A', 'W'),
        XWMA = MAKEFOURCC('A', 'M', 'W', 'X'),
        DPDS = MAKEFOURCC('s', 'd', 'p', 'd')
#else
        RIFF = MAKEFOURCC('R', 'I', 'F', 'F'),
        DATA = MAKEFOURCC('d', 'a', 't', 'a'),
        FMT  = MAKEFOURCC('f', 'm', 't', ' '),
        WAVE = MAKEFOURCC('W', 'A', 'V', 'E'),
        XWMA = MAKEFOURCC('X', 'W', 'M', 'A'),
        DPDS = MAKEFOURCC('D', 'P', 'D', 'S')
#endif
    };

    struct FindChunk
    {
        Result operator()(std::ifstream& fileStream, Fourcc fourcc, std::streamsize& chunkSize,
                          DWORD& chunkDataPosition) const;
    };

    struct ReadChunkData
    {
        Result operator()(std::ifstream& fileStream, void* buffer, const std::streamsize& bufferSize,
                          DWORD bufferOffset) const;
    };
} // namespace Audio
