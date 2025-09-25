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
        void Initialize();
        void Finalize();
        void ClearVoicePool();

        void LoadSound(const std::string& key, const File::GuidRef& guid);

        AudioHandle Play(const std::string& key, Group group = GROUP_EFFECT, bool isLoop = false);
        void        Stop(const AudioHandle& handle);

    private:
        System _system;

        std::unordered_map<std::string, Source> _sources;
        std::unordered_map<Group, GroupHandle>  _groups;
    };
}