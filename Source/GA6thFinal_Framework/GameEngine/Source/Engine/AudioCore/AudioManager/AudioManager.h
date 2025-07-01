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
        EManager(const EManager&) = delete;
        EManager& operator=(const EManager&) = delete;
        EManager(EManager&& other) noexcept;
        EManager& operator=(EManager&& other) noexcept;

        void                          Initialize();
        static std::shared_ptr<Sound> CreateSoundFromWave(const std::filesystem::path& filePath);
        std::shared_ptr<SoundPlayer>  CreatePlayer(const std::shared_ptr<Sound>& sound) const;

    private:
        winrt::com_ptr<IXAudio2> _xAudio2;
        IXAudio2MasteringVoice*  _masteringVoice = nullptr;
    };
} // namespace Audio