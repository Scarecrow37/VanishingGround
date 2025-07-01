#pragma once

namespace Audio
{
    class Sound;
    class SoundPlayer;
    class Handle;

    class EManager
    {
        friend class EngineCores;

        EManager();

    public:
        ~EManager();
        EManager(const EManager&)            = delete;
        EManager& operator=(const EManager&) = delete;
        EManager(EManager&& other) noexcept;
        EManager& operator=(EManager&& other) noexcept;

        void                 Initialize();
        static Sound         CreateSoundFromWave(const std::filesystem::path& filePath , bool isLoop = false);

        [[nodiscard]] Handle Play(const Sound& sound);
        void                 Stop(const Handle& handle);

        [[nodiscard]] bool IsValidHandle(const Handle& handle) const;

    private:
        [[nodiscard]] WaveFormatHash GetWaveFormatHash(const WAVEFORMATEX& waveFormat) const;

        winrt::com_ptr<IXAudio2> _xAudio2;
        IXAudio2MasteringVoice*  _masteringVoice = nullptr;

        std::unordered_map<WaveFormatHash, std::vector<Generation>>           _generationMap;
        std::unordered_map<WaveFormatHash, std::vector<IXAudio2SourceVoice*>> _sourceVoices;
    };
} // namespace Audio