#pragma once
class TestAudioComponent : public Component
{
    USING_PROPERTY(TestAudioComponent)

public:
    TestAudioComponent();
    ~TestAudioComponent() override;

public:
    REFLECT_PROPERTY(Path)
 
    GETTER(std::string, Path) { return _filePath.string(); }
    SETTER(std::string, Path) { _filePath = value; }
    PROPERTY(Path)

    void ImGuiDrawPropertysEvent() override;

protected:
    File::Path          _filePath;
    Audio::Group        _group;
    Audio::AudioHandle  _hAudio;
    float               _masterVolume = 1.0f;

    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TestAudioComponent)
};
