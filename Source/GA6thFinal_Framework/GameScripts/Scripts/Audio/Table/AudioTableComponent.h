#pragma once

class AudioTableComponent : public Component
{
    USING_PROPERTY(AudioTableComponent)

public:
    AudioTableComponent();

public:
    void Play(const std::string& key);
    void Stop(const std::string& key);

protected:
    void ImGuiDrawPropertysEvent() override;
    void Reset() override;

private:
    void LoadAudio();
    void LoadAudio(const std::string& key, const File::Path& path);

    void PlaySelectedAudio();
    void StopSelectedAudio();


protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::unordered_map<std::string, std::string> AudioMappingKeys;
    REFLECT_FIELDS_END(AudioTableComponent)

private:
    std::string _newKey;
    std::string _newPathString;

    std::string _selectedAudioKey;

    std::unordered_map<std::string, Audio::Source> _audioSources;
    std::unordered_map<std::string, Audio::Handle> _audioHandles;
};