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
    Result FindChunk::operator()(std::ifstream& fileStream, const Fourcc fourcc, DWORD& chunkSize,
                                 DWORD& chunkDataPosition) const
    {
        Result result = AUDIO_ERROR_SUCCESS;

        // 파일 포인터를 파일의 시작으로 설정.
        fileStream.seekg(0, std::ios::beg);

        // 초기화
        DWORD chunkType     = 0;
        DWORD chunkDataSize = 0;
        DWORD riffDataSize  = 0;
        DWORD bytesRead     = 0;
        DWORD offset        = 0;

        while (result == AUDIO_ERROR_SUCCESS)
        {
            fileStream.read(reinterpret_cast<char*>(&chunkType), sizeof(DWORD));
            bytesRead += sizeof(DWORD);

            fileStream.read(reinterpret_cast<char*>(&chunkDataSize), sizeof(DWORD));
            bytesRead += sizeof(DWORD);

            switch (chunkType)
            {
            case RIFF: // RIFF 청크 (처음 청크)
            {
                riffDataSize  = chunkDataSize;
                chunkDataSize = 4; // 파일 타입 크기 ("WAVE")

                DWORD fileType = 0;
                fileStream.read(reinterpret_cast<char*>(&fileType), sizeof(DWORD));
                bytesRead += sizeof(DWORD);

                if (fileType != WAVE)
                    return AUDIO_ERROR_INVALID_FILE_FORMAT;
            }
            break;
            default: // 이후 청크
                fileStream.seekg(chunkDataSize, std::ios::cur);
            }

            offset += sizeof(DWORD) * 2;

            if (chunkType == fourcc)
            {
                chunkSize         = chunkDataSize;
                chunkDataPosition = offset;
                return AUDIO_ERROR_SUCCESS;
            }

            offset += chunkDataSize;

            if (bytesRead >= riffDataSize)
                return AUDIO_ERROR_FAIL;
        }
        return result;
    }

    Result ReadChunkData::operator()(std::ifstream& fileStream, void* buffer, const DWORD bufferSize,
                                     const DWORD bufferOffset) const
    {
        Result result = AUDIO_ERROR_SUCCESS;

        fileStream.seekg(bufferOffset, std::ios::beg);

        fileStream.read(static_cast<char*>(buffer), bufferSize);

        if (fileStream.gcount() != static_cast<std::streamsize>(bufferSize))
            result = AUDIO_ERROR_FILE_READ_FAIL;

        return result;
    }
} // namespace Audio
