#include "pch.h"
#include "AudioSound.h"

namespace Audio
{
    Sound::Sound(const WAVEFORMATEXTENSIBLE& format, const XAUDIO2_BUFFER& buffer): _format(format), _buffer(buffer)
    {
        
    }

    Sound::~Sound()
    {
        delete[] _buffer.pAudioData;
    }
}