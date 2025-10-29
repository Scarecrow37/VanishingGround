#pragma once

class AudioTableComponent : public Component
{
    USING_PROPERTY(AudioTableComponent)

public:
    AudioTableComponent();

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

    std::unordered_map<std::string, std::vector<Audio::AudioHandle>> _audioHandles;
};