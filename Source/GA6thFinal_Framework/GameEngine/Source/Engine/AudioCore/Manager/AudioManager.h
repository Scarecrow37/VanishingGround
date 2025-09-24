#pragma once

namespace Audio
{
    class Manager
    {
    public:
        void Initialize();
        void Finalize();
        void ClearVoicePool();

        void LoadSound(const std::string& key, const File::GuidRef& guid);

        AudioHandle Play(const std::string& key, bool isLoop = false);
        void   Stop(const AudioHandle& handle);

    private:
        System _system;

        std::unordered_map<std::string, Source> _sources;
    };
}