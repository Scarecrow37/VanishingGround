#pragma once

class AudioComponent : public Component
{
    USING_PROPERTY(AudioComponent)

public:
    AudioComponent();

public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return _guidRef.ToPath().string(); }
    PROPERTY(FilePath)

    void Play();
    void Stop();

protected:
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

private:
    void LoadAudio();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string Guid;
    REFLECT_FIELDS_END(AudioComponent)

private:
    File::GuidRef _guidRef;
    std::optional<Audio::Source> _audioSource;
    Audio::Handle                _audioHandle;
};