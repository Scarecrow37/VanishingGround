#include "pch.h"
#include <xaudio2.h>

namespace Audio
{
    namespace
    {
#ifdef _XBOX
        constexpr FOURCC RIFF = MAKEFOURCC('F', 'F', 'I', 'R');
        constexpr FOURCC DATA = MAKEFOURCC('a', 't', 'a', 'd');
        constexpr FOURCC FMT  = MAKEFOURCC(' ', 't', 'm', 'f');
        constexpr FOURCC WAVE = MAKEFOURCC('E', 'V', 'A', 'W');
        constexpr FOURCC XWMA = MAKEFOURCC('A', 'M', 'W', 'X');
        constexpr FOURCC DPDS = MAKEFOURCC('s', 'd', 'p', 'd');
#else
        constexpr FOURCC RIFF = MAKEFOURCC('R', 'I', 'F', 'F');
        constexpr FOURCC DATA = MAKEFOURCC('d', 'a', 't', 'a');
        constexpr FOURCC FMT  = MAKEFOURCC('f', 'm', 't', ' ');
        constexpr FOURCC WAVE = MAKEFOURCC('W', 'A', 'V', 'E');
        constexpr FOURCC XWMA = MAKEFOURCC('X', 'W', 'M', 'A');
        constexpr FOURCC DPDS = MAKEFOURCC('D', 'P', 'D', 'S');
#endif

        struct FindChunk
        {
            std::pair<std::streamsize, DWORD> operator()(std::ifstream& fileStream, const FOURCC fourcc) const
            {
                static constexpr std::streamsize FOURCC_COUNT          = sizeof(FOURCC);
                static constexpr std::streamsize CHUNK_DATA_SIZE_COUNT = sizeof(DWORD);

                // 초기화
                FOURCC                    format;

                std::streamsize           chunkDataSize      = 0;
                std::streamsize           offset             = 0;

                // 파일 포인터를 파일의 시작으로 설정.
                fileStream.seekg(0, std::ios::beg);

                while (fileStream.eof())
                {
                    fileStream.read(reinterpret_cast<char*>(&format), FOURCC_COUNT);
                    fileStream.read(reinterpret_cast<char*>(&chunkDataSize), CHUNK_DATA_SIZE_COUNT);

                    if (format == RIFF)
                    {
                        chunkDataSize = FOURCC_COUNT;
                        fileStream.seekg(FOURCC_COUNT, std::ios::cur);
                    }
                    else
                    {
                        fileStream.seekg(chunkDataSize, std::ios::cur);
                    }

                    offset += FOURCC_COUNT + CHUNK_DATA_SIZE_COUNT;

                    if (format == fourcc)
                    {
                        return std::make_pair(chunkDataSize, offset);
                    }

                    offset += chunkDataSize;
                }
                throw AudioException("Chunk not found: " + std::to_string(fourcc));
            }
        };

        struct ReadChunkData
        {
            void operator()(std::ifstream& fileStream, void* buffer, const std::streamsize& bufferSize,
                            const DWORD bufferOffset) const
            {
                fileStream.seekg(bufferOffset, std::ios::beg);
                fileStream.read(static_cast<char*>(buffer), bufferSize);
                if (fileStream.gcount() != bufferSize)
                    throw AudioException("Failed to read chunk data: " + std::to_string(bufferSize));
            }
        };
    } // namespace





} // namespace Audio