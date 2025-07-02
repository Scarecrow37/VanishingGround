#pragma once

namespace Audio
{
    class Source;
    class SoundPlayer;
    class Handle;

    /// <summary>
    /// 오디오 소스를 관리하고 재생, 정지, 생성 등의 기능을 제공하는 클래스입니다.
    /// </summary>
    class EManager
    {
        friend class EngineCores;
        friend class IncreaseGenerationCallback;

        struct SourceVoice
        {
            Generation                 Generation;
            IncreaseGenerationCallback Callback;
            IXAudio2SourceVoice*       Voice;
        };

        EManager();

    public:
        ~EManager();
        EManager(const EManager&)            = delete;
        EManager& operator=(const EManager&) = delete;
        EManager(EManager&& other) noexcept;
        EManager& operator=(EManager&& other) noexcept;

        /// <summary>
        /// 초기화 작업을 수행합니다. 최초 1회 호출되어야 합니다.
        /// 호출하기 전 CoInitializeEx()가 MTA로 호출되어야 합니다.
        /// </summary>
        void Initialize();

        /// <summary>
        /// 지정된 파일 경로에서 웨이브 파일로부터 사운드 소스를 생성합니다.
        /// </summary>
        /// <param name="filePath">웨이브 파일의 경로를 나타내는 std::filesystem::path 객체입니다.</param>
        /// <param name="isLoop">사운드가 반복 재생될지 여부를 지정하는 불리언 값입니다. 기본값은 false입니다.</param>
        /// <returns>생성된 사운드 소스를 나타내는 Source 객체를 반환합니다.</returns>
        static Source CreateSoundFromWave(const std::filesystem::path& filePath, bool isLoop = false);

        /// <summary>
        /// 사운드 소스를 재생하고 핸들을 반환합니다.
        /// </summary>
        /// <param name="sound">재생할 사운드 소스입니다.</param>
        /// <returns>재생 중인 사운드를 제어할 수 있는 Handle 객체를 반환합니다.</returns>
        [[nodiscard]] Handle Play(const Source& sound);

        /// <summary>
        /// 지정된 핸들에 대한 재생 작업을 중지합니다.
        /// </summary>
        /// <param name="handle">중지할 작업을 나타내는 Handle 객체의 상수 참조입니다.</param>
        void Stop(const Handle& handle);

        /// <summary>
        /// Handle 객체가 유효한지 확인합니다.
        /// </summary>
        /// <param name="handle">유효성을 검사할 Handle 객체입니다.</param>
        /// <returns>핸들이 유효하면 true, 그렇지 않으면 false를 반환합니다.</returns>
        [[nodiscard]] bool IsValidHandle(const Handle& handle) const noexcept;

    private:
        [[nodiscard]] WaveFormatHash GetWaveFormatHash(const WAVEFORMATEX& waveFormat) const;
        void                         ReleaseVoice(const Handle& handle);

        winrt::com_ptr<IXAudio2> _xAudio2;
        IXAudio2MasteringVoice*  _masteringVoice = nullptr;

        std::unordered_map<WaveFormatHash, std::vector<SourceVoice>> _sourceVoices;
    };
} // namespace Audio