#pragma once

namespace Audio
{
    enum Group : unsigned char
    {
        GROUP_EFFECT,
        GROUP_BGM,
        GROUP_MAX
    };

    enum Effect : unsigned char
    {
        EFFECT_REVERB,
        EFFECT_FADE,
        EFFECT_MAX
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

        void EnableEffect(Effect effect, Group group);
        void DisableEffect(Effect effect);

    private:
        System _system;

        std::unordered_map<std::string, Source> _sources;
        std::unordered_map<Group, GroupHandle>  _groups;
        std::unordered_map<Effect, EffectHandle> _effects;
    };
} // namespace Audio