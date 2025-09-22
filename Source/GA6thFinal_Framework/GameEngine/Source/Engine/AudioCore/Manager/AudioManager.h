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

        Handle Play(const std::string& key);
        void   Stop(const Handle& handle);

    private:
        System _system;

        std::unordered_map<std::string, Source> _sources;
    };
}