#pragma once
#include "Callback.h"

namespace Audio
{
    class Source;
    class SoundPlayer;
    class AudioHandle;
    class GroupHandle;

    /// <summary>
    /// 오디오 소스를 관리하고 재생, 정지, 생성 등의 기능을 제공하는 클래스입니다.
    /// </summary>
    class System
    {
        static constexpr size_t MAX_POOL_SIZE = 64;

        struct SourceVoice
        {
            Generation           Generation;
            Callback             Callback;
            IXAudio2SourceVoice* Voice;
        };

        using VoicePool = std::vector<SourceVoice>;

        struct SubmixVoice
        {
            Generation           Generation;
            UINT32               Channels;
            UINT32               SampleRate;
            IXAudio2SubmixVoice* Voice;
            std::list<AudioHandle> AttachedVoices;
        };

        using GroupPool = std::vector<SubmixVoice>;

    public:
        System();
        System(const System&)            = delete;
        System& operator=(const System&) = delete;
        System(System&& other) noexcept;
        System& operator=(System&& other) noexcept;
        ~System();

        /// <summary>
        /// 초기화 작업을 수행합니다. 최초 1회 호출되어야 합니다.
        /// 호출하기 전 CoInitializeEx()가 MTA로 호출되어야 합니다.
        /// </summary>
        void Initialize(bool isDebug = false);

        /// <summary>
        /// 객체 또는 프로세스를 종료하거나 정리합니다.
        /// </summary>
        void Finalize();

        /// <summary>
        /// 음성 풀을 초기화하여 모든 음성 리소스를 제거합니다.
        /// </summary>
        void ClearVoicePool();

        /// <summary>
        /// 그룹 풀을 초기화합니다.
        /// </summary>
        void ClearGroupPool();

        /// <summary>
        /// 디버그 모드를 활성화합니다.
        /// </summary>
        void TurnOnDebugMode() const;

        /// <summary>
        /// 디버그 모드를 끕니다.
        /// </summary>
        void TurnOffDebugMode() const;

        /// <summary>
        /// 지정된 파일 경로에서 웨이브 파일로부터 사운드 소스를 생성합니다.
        /// </summary>
        /// <param name="filePath">웨이브 파일의 경로를 나타내는 std::filesystem::path 객체입니다.</param>
        /// <returns>생성된 사운드 소스를 나타내는 Source 객체를 반환합니다.</returns>
        static Source CreateSoundFromWave(const std::filesystem::path& filePath);

        /// <summary>
        /// 사운드 소스를 재생하고 핸들을 반환합니다.
        /// </summary>
        /// <param name="sound">재생할 사운드 소스입니다.</param>
        /// <param name="isLoop">반복 재생 여부입니다.</param>
        /// <param name="groups">사운드가 재생될 그룹들입니다.</param>
        /// <returns>재생 중인 사운드를 제어할 수 있는 AudioHandle 객체를 반환합니다.</returns>
        [[nodiscard]] AudioHandle Play(const Source& sound, std::span<GroupHandle> groups = {},
                                       bool isLoop = false);

        /// <summary>
        /// 지정된 핸들에 대한 재생 작업을 중지합니다.
        /// </summary>
        /// <param name="handle">중지할 작업을 나타내는 AudioHandle 객체의 상수 참조입니다.</param>
        void Stop(const AudioHandle& handle);

        /// <summary>
        /// AudioHandle 객체가 유효한지 확인합니다.
        /// </summary>
        /// <param name="handle">유효성을 검사할 AudioHandle 객체입니다.</param>
        /// <returns>핸들이 유효하면 true, 그렇지 않으면 false를 반환합니다.</returns>
        [[nodiscard]] bool IsValidHandle(const AudioHandle& handle) const noexcept;

        /// <summary>
        /// GroupHandle이 유효한 핸들인지 확인합니다.
        /// </summary>
        /// <param name="handle">검사할 GroupHandle 객체입니다.</param>
        /// <returns>핸들이 유효하면 true, 그렇지 않으면 false를 반환합니다.</returns>
        [[nodiscard]] bool IsValidHandle(const GroupHandle& handle) const noexcept;

        GroupHandle CreateGroup(UINT32 channels = 2, UINT32 sampleRate = 44100);

        void ReleaseGroup(const GroupHandle& handle);

    private:
        void ReleaseVoice(const AudioHandle& handle);
        void DetachOutput(const AudioHandle& handle) const;

        IXAudio2*               _xAudio2;
        IXAudio2MasteringVoice* _masteringVoice = nullptr;

        std::unordered_map<WaveFormatHash, VoicePool> _voicePools;
        GroupPool                                     _groupPool;

        struct OnBufferEnd
        {
            explicit OnBufferEnd(System* system);

            void operator()(const AudioHandle& handle) const;

            System* System;
        };
    };
} // namespace Audio