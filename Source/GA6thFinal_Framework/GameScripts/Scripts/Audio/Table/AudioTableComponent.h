#pragma once

class AudioTableComponent : public Component
{
    USING_PROPERTY(AudioTableComponent)

public:
    AudioTableComponent();

public:
    Audio::Handle Play(const std::string& key) const;

protected:
    void ImGuiDrawPropertysEvent() override;
    void Reset() override;

private:
    void LoadAudio();
    void LoadAudio(const std::string& key, const File::Guid& guid);

    void PlaySelectedAudio();
    void StopSelectedAudio();


protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::unordered_map<std::string, std::string> AudioMappingKeys;
    REFLECT_FIELDS_END(AudioTableComponent)

private:
    std::string _newKey;
    std::string _newPathString;

    std::string   _selectedAudioKey;
    Audio::Handle _selectedAudioHandle;

    std::unordered_map<std::string, Audio::Source> _audioSources;
};