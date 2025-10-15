#pragma once

namespace Audio
{
    enum Group : unsigned char
    {
        GROUP_EFFECT,
        GROUP_BGM,
        GROUP_MAX
    };

    class Manager
    {
    public:
        /// <summary>
        /// 초기화 작업을 수행합니다.
        /// </summary>
        void Initialize();

        /// <summary>
        /// 객체 또는 프로세스를 종료하거나 정리합니다.
        /// </summary>
        void Finalize();

        /// <summary>
        /// 음성 풀을 초기화합니다.
        /// </summary>
        void ClearVoicePool();

        /// <summary>
        /// 지정된 키와 파일 GUID를 사용하여 사운드를 로드합니다.
        /// </summary>
        /// <param name="key">사운드를 식별하는 문자열 키입니다.</param>
        /// <param name="guid">사운드 파일을 참조하는 File::Guid 객체입니다.</param>
        void LoadSound(const std::string& key, const File::Guid& guid);

        /// <summary>
        /// 지정된 키에 해당하는 오디오를 재생합니다.
        /// </summary>
        /// <param name="key">재생할 오디오의 키입니다.</param>
        /// <param name="group">오디오가 속할 그룹입니다. 기본값은 GROUP_EFFECT입니다.</param>
        /// <param name="isLoop">오디오를 반복 재생할지 여부입니다. 기본값은 false입니다.</param>
        /// <returns>재생된 오디오를 제어할 수 있는 AudioHandle 객체를 반환합니다.</returns>
        AudioHandle Play(const std::string& key, Group group = GROUP_EFFECT, bool isLoop = false);

        /// <summary>
        /// 지정된 오디오 핸들의 재생을 중지합니다.
        /// </summary>
        /// <param name="handle">중지할 오디오를 나타내는 AudioHandle 참조입니다.</param>
        void Stop(const AudioHandle& handle);

        /// <summary>
        /// 오디오 핸들의 볼륨을 설정합니다.
        /// </summary>
        /// <param name="handle">볼륨을 설정할 오디오 핸들입니다.</param>
        /// <param name="volume">설정할 볼륨 값입니다. (0.0에서 1.0 사이의 실수)</param>
        void SetVolume(const AudioHandle& handle, float volume) const;

        /// <summary>
        /// 그룹의 볼륨을 설정합니다.
        /// </summary>
        /// <param name="group">볼륨을 설정할 그룹입니다.</param>
        /// <param name="volume">설정할 볼륨 값입니다.</param>
        void SetVolume(Group group, float volume) const;

        /// <summary>
        /// 볼륨을 설정합니다.
        /// </summary>
        /// <param name="volume">설정할 볼륨 값입니다.</param>
        void SetVolume(float volume) const;

        void FadeIn() const;
        void FadeOut() const;

        void ReverbOn() const;
        void ReverbOff() const;

    private:
        System _system;

        std::unordered_map<std::string, Source> _sources;
        std::unordered_map<Group, GroupHandle>  _groups;
        ReverbHandle                             _reverbHandle;
        FadeHandle                               _fadeHandle;
    };
} // namespace Audio