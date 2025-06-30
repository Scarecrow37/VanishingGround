#pragma once

namespace Audio
{
    class Sound;
    class SoundPlayer;

    class EManager
    {
        friend class EngineCores;

        EManager();
    public:
        ~EManager();

        Result Initialize();
        Result CreateSoundFromWave(const std::filesystem::path& filePath, Sound** sound);
        Result CreatePlayer(const Sound& sound, SoundPlayer** soundPlayer);

    private:
        winrt::com_ptr<IXAudio2> _xAudio2;
        IXAudio2MasteringVoice*  _masteringVoice = nullptr;
    };
}