#pragma once
#include "Callback.h"

namespace Audio
{
    enum class EffectType : unsigned char;
    class Source;
    class SoundPlayer;
    class AudioHandle;
    class GroupHandle;
    class EffectHandle;
    class FadeHandle;

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

        struct GroupVoice
        {
            Generation             Generation;
            UINT32                 Channels;
            UINT32                 SampleRate;
            IXAudio2SubmixVoice*   Voice;
            std::list<AudioHandle> AttachedVoices;
        };

        struct EffectVoice
        {
            EffectType             Type;
            Generation             Generation;
            UINT32                 Channels;
            UINT32                 SampleRate;
            IXAudio2SubmixVoice*   Voice;
            std::list<GroupHandle> AttachedVoices;
        };

        using GroupPool = std::vector<GroupVoice>;
        using EffectPool = std::vector<EffectVoice>;

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
        /// 이펙트 풀을 초기화합니다.
        /// </summary>
        void ClearEffectPool();

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
        /// <exception cref="InvalidHandleException">group에 들어온 GroupHandle이 유효하지 않습니다.</exception>
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

        /// <summary>
        /// EffectHandle이 유효한 핸들인지 확인합니다.
        /// </summary>
        /// <param name="handle">검사할 EffectHandle 객체입니다.</param>
        /// <returns>핸들이 유효하면 true, 그렇지 않으면 false를 반환합니다.</returns>
        [[nodiscard]] bool IsValidHandle(const EffectHandle& handle) const noexcept;

        /// <summary>
        /// 새 오디오 그룹을 생성합니다.
        /// </summary>
        /// <param name="channels">오디오 그룹의 채널 수입니다. 기본값은 2입니다.</param>
        /// <param name="sampleRate">오디오 그룹의 샘플레이트(Hz)입니다. 기본값은 44100입니다.</param>
        /// <returns>생성된 오디오 그룹을 나타내는 GroupHandle입니다.</returns>
        GroupHandle CreateGroup(UINT32 channels = 2, UINT32 sampleRate = 44100);

        /// <summary>
        /// 지정된 그룹 핸들을 해제합니다.
        /// </summary>
        /// <param name="handle">해제할 그룹을 나타내는 GroupHandle 참조입니다.</param>
        void ReleaseGroup(const GroupHandle& handle);

        /// <summary>
        /// 마스터 볼륨을 설정합니다.
        /// </summary>
        /// <param name="volume">설정할 마스터 볼륨 값입니다. 값은 0 ~ 1 사이의 값입니다.</param>
        void SetVolume(float volume) const;

        /// <summary>
        /// 지정된 그룹의 볼륨을 설정합니다.
        /// </summary>
        /// <param name="handle">볼륨을 설정할 그룹을 식별하는 GroupHandle 참조입니다.</param>
        /// <param name="volume">설정할 볼륨 값입니다. 값은 0 ~ 1 사이의 값입니다.</param>
        void SetVolume(const GroupHandle& handle, float volume) const;

        /// <summary>
        /// 오디오 핸들의 볼륨을 설정합니다.
        /// </summary>
        /// <param name="handle">볼륨을 설정할 오디오 핸들입니다.</param>
        /// <param name="volume">설정할 볼륨 값입니다. 값은 0 ~ 1 사이의 값입니다.</param>
        void SetVolume(const AudioHandle& handle, float volume) const;

        /// <summary>
        /// 리버브 효과를 생성합니다.
        /// </summary>
        /// <param name="channels">오디오 채널 수입니다. 기본값은 2입니다.</param>
        /// <param name="sampleRate">오디오 샘플레이트(Hz)입니다. 기본값은 44100입니다.</param>
        /// <returns>생성된 리버브 효과를 나타내는 ReverbHandle 객체를 반환합니다.</returns>
        ReverbHandle CreateReverbEffect(UINT32 channels = 2, UINT32 sampleRate = 44100);

        /// <summary>
        /// 페이드 효과를 생성합니다.
        /// </summary>
        /// <param name="parameter">페이드 효과에 사용할 파라미터입니다.</param>
        /// <param name="channels">오디오 채널 수입니다. 기본값은 2입니다.</param>
        /// <param name="sampleRate">오디오 샘플레이트(Hz)입니다. 기본값은 44100입니다.</param>
        /// <returns>생성된 페이드 효과를 나타내는 FadeHandle입니다.</returns>
        FadeHandle CreateFadeEffect(FadeInitParameter parameter, UINT32 channels = 2, UINT32 sampleRate = 44100);

        /// <summary>
        /// 리버브 효과의 파라미터 값을 설정합니다.
        /// </summary>
        /// <param name="handle">파라미터를 설정할 리버브 효과의 핸들입니다.</param>
        /// <param name="parameter">설정할 리버브 파라미터 값입니다.</param>
        void SetEffectParameter(const ReverbHandle& handle, ReverbParameter parameter) const;

        /// <summary>
        /// 효과 파라미터를 설정합니다.
        /// </summary>
        /// <param name="handle">파라미터를 설정할 페이드 효과 핸들입니다.</param>
        /// <param name="parameter">설정할 페이드 파라미터 값입니다.</param>
        void SetEffectParameter(const FadeHandle& handle, FadeParameter parameter) const;

        /// <summary>
        /// EffectHandle 객체에 해당하는 이펙트를 해제합니다.
        /// </summary>
        /// <param name="handle">해제할 이펙트를 나타내는 EffectHandle 객체의 상수 참조입니다.</param>
        void ReleaseEffect(const EffectHandle& handle);

        /// <summary>
        /// 효과를 그룹에 연결합니다.
        /// </summary>
        /// <param name="effectHandle">연결할 효과를 나타내는 EffectHandle 참조입니다.</param>
        /// <param name="groupHandle">효과를 연결할 그룹을 나타내는 GroupHandle 참조입니다.</param>
        void AttachEffect(const EffectHandle& effectHandle, const GroupHandle& groupHandle);

        /// <summary>
        /// 지정된 이펙트 핸들을 분리합니다.
        /// </summary>
        /// <param name="handle">분리할 이펙트의 핸들입니다.</param>
        void DetachEffect(const EffectHandle& handle);

        /// <summary>
        /// 지정된 이펙트를 활성화합니다.
        /// </summary>
        /// <param name="handle">활성화할 이펙트의 핸들입니다.</param>
        void EnableEffect(const EffectHandle& handle) const;

        /// <summary>
        /// 지정된 이펙트를 비활성화합니다.
        /// </summary>
        /// <param name="handle">비활성화할 이펙트의 핸들입니다.</param>
        void DisableEffect(const EffectHandle& handle) const;

    private:
        void ReleaseVoice(const AudioHandle& handle);
        void DetachOutput(const AudioHandle& handle) const;
        void DetachOutput(const GroupHandle& handle) const;

        IXAudio2*               _xAudio2;
        IXAudio2MasteringVoice* _masteringVoice = nullptr;

        std::unordered_map<WaveFormatHash, VoicePool> _voicePools;
        GroupPool                                     _groupPool;
        EffectPool                                    _effectPool;

        struct OnBufferEnd
        {
            explicit OnBufferEnd(System* system);

            void operator()(const AudioHandle& handle) const;

            System* System;
        };
    };
} // namespace Audio