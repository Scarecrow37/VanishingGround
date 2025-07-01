#include "pch.h"
#include "SoundPlayer.h"

#include "Engine/AudioCore/Sound/AudioSound.h"

namespace Audio
{
    SoundPlayer::SoundPlayer(IXAudio2SourceVoice* voice, const std::shared_ptr<const Sound>& sound)
        : _sourceVoice(voice), _sound(sound)
    {
    }

    SoundPlayer::SoundPlayer(SoundPlayer&& other) noexcept : _sourceVoice(other._sourceVoice)
    {
        other._sourceVoice = nullptr;
    }

    SoundPlayer& SoundPlayer::operator=(SoundPlayer&& other) noexcept
    {
        if (this == &other)
            return *this;
        _sourceVoice       = other._sourceVoice;
        other._sourceVoice = nullptr;
        return *this;
    }

    SoundPlayer::~SoundPlayer()
    {
        if (_sourceVoice != nullptr)
        {
            _sourceVoice->DestroyVoice();
            _sourceVoice = nullptr;
        }
    }

    void SoundPlayer::Play() const
    {
        Submit();
        ThrowIfFailed()(_sourceVoice->Start(0), "Failed to start sound player.");
    }

    void SoundPlayer::Stop() const
    {
        ThrowIfFailed()(_sourceVoice->Stop(0), "Failed to stop sound player.");
        ThrowIfFailed()(_sourceVoice->FlushSourceBuffers(), "Failed to flush source buffers.");
    }

    void SoundPlayer::Resume() const
    {
        ThrowIfFailed()(_sourceVoice->Start(0), "Failed to resume sound player.");
    }

    void SoundPlayer::Pause() const
    {
        ThrowIfFailed()(_sourceVoice->Stop(0), "Failed to Pause sound player.");
    }

    void SoundPlayer::Submit() const
    {
        ThrowIfFailed()(_sourceVoice->SubmitSourceBuffer(&_sound->_buffer), "Failed to submit source buffer.");
    }
} // namespace Audio