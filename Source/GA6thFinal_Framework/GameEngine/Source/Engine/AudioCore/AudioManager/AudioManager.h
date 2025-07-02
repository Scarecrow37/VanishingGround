#pragma once

namespace Audio
{
    class Source;
    class SoundPlayer;
    class Handle;

    class EManager
    {
        friend class EngineCores;
        friend class IncreaseGenerationCallback;

        struct SourceVoice
        {
            Generation Generation;
            IncreaseGenerationCallback Callback;
            IXAudio2SourceVoice* Voice;
        };

        EManager();

    public:
        ~EManager();
        EManager(const EManager&)            = delete;
        EManager& operator=(const EManager&) = delete;
        EManager(EManager&& other) noexcept;
        EManager& operator=(EManager&& other) noexcept;

        void                 Initialize();
        static Source         CreateSoundFromWave(const std::filesystem::path& filePath , bool isLoop = false);

        [[nodiscard]] Handle Play(const Source& sound);
        void                 Stop(const Handle& handle);

        [[nodiscard]] bool IsValidHandle(const Handle& handle) const;

    private:
        [[nodiscard]] WaveFormatHash GetWaveFormatHash(const WAVEFORMATEX& waveFormat) const;
        void                         ReleaseVoice(const Handle& handle);

        winrt::com_ptr<IXAudio2> _xAudio2;
        IXAudio2MasteringVoice*  _masteringVoice = nullptr;

        std::unordered_map<WaveFormatHash, std::vector<SourceVoice>> _sourceVoices;
    };
} // namespace Audio