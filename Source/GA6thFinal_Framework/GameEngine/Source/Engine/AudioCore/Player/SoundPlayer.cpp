#include "pch.h"
#include "SoundPlayer.h"

Audio::SoundPlayer::SoundPlayer(SoundPlayer&& other) noexcept : _sourceVoice(other._sourceVoice)
{
    other._sourceVoice = nullptr;
}

Audio::SoundPlayer& Audio::SoundPlayer::operator=(SoundPlayer&& other) noexcept
{
    if (this == &other)
        return *this;
    _sourceVoice       = other._sourceVoice;
    other._sourceVoice = nullptr;
    return *this;
}

Audio::SoundPlayer::~SoundPlayer()
{
    if (_sourceVoice != nullptr)
    {
        _sourceVoice->DestroyVoice();
        _sourceVoice = nullptr;
    }
}