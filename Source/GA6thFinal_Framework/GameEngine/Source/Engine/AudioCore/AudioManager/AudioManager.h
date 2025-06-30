#pragma once

namespace Audio
{
    class Sound;
    class EManager
    {
        friend class EngineCores;

        EManager();
    public:
        ~EManager();

        Result Initialize();
        Result CreateSoundFromWave(const std::filesystem::path& filePath, Sound** sound);

    private:
        winrt::com_ptr<IXAudio2> _xAudio2;
        IXAudio2MasteringVoice*  _masteringVoice = nullptr;
    };
}