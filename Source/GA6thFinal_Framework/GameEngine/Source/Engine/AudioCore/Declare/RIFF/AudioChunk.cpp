#include "pch.h"
#include "AudioChunk.h"

// RIFF 헤더 구조체 정의
// Offset | Size | 내용
// -------------------- -
//      0 |    4 | "RIFF"           (chunk ID)
//      4 |    4 | 파일 크기 - 8     (chunk size)
//      8 |    4 | "WAVE"           (format type)
//
//     12 |    4 | "fmt "           (subchunk1 ID)
//     16 |    4 | fmt 크기(보통 16)
//     20 |    2 | 오디오 포맷(1 = PCM)
//     22 |    2 | 채널 수(mono = 1, stereo = 2)
//     24 |    4 | 샘플레이트
//     28 |    4 | 바이트레이트
//     32 |    2 | 블록 정렬
//     34 |    2 | 비트 깊이
//
//     36 |    4 | "data"           (subchunk2 ID)
//     40 |    4 | 데이터 크기
//     44 |    N | 실제 오디오 데이터

namespace Audio
{
    std::pair<std::streamsize, DWORD> FindChunk::operator()(std::ifstream& fileStream, const Fourcc fourcc) const
    {
        // 초기화
        constexpr std::streamsize fourccCount = sizeof(Fourcc);
        Fourcc                    format;

        constexpr std::streamsize chunkDataSizeCount = sizeof(DWORD);
        std::streamsize           chunkDataSize      = 0;
        std::streamsize           offset             = 0;

        // 파일 포인터를 파일의 시작으로 설정.
        fileStream.seekg(0, std::ios::beg);

        while (fileStream.eof())
        {
            fileStream.read(reinterpret_cast<char*>(&format), fourccCount);
            fileStream.read(reinterpret_cast<char*>(&chunkDataSize), chunkDataSizeCount);

            if (format == RIFF)
            {
                chunkDataSize = fourccCount;
                fileStream.seekg(fourccCount, std::ios::cur);
            }
            else
            {
                fileStream.seekg(chunkDataSize, std::ios::cur);
            }

            offset += fourccCount + chunkDataSizeCount;

            if (format == fourcc)
            {
                return std::make_pair(chunkDataSize, offset);
            }

            offset += chunkDataSize;
        }
        throw AudioException("Chunk not found: " + std::to_string(fourcc));
    }

    void ReadChunkData::operator()(std::ifstream& fileStream, void* buffer, const std::streamsize& bufferSize,
                                     const DWORD bufferOffset) const
    {
        fileStream.seekg(bufferOffset, std::ios::beg);

        fileStream.read(static_cast<char*>(buffer), bufferSize);

        if (fileStream.gcount() != bufferSize)
            throw AudioException("Failed to read chunk data: " + std::to_string(bufferSize));
    }
} // namespace Audio