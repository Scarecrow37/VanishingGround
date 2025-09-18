#pragma once

namespace Timeline
{
    class AudioEventContext : public EventContext
    {
    public:
        USING_PROPERTY(AudioEventContext)
        AudioEventContext();
        ~AudioEventContext() override;

    public:
        REFLECT_PROPERTY(Path, Volume) 

        GETTER_ONLY(std::string, Path) { return _audioGuid.ToPath().string(); }
        PROPERTY(Path)

        GETTER(float, Volume) { return _volume; }
        SETTER(float, Volume) { _volume = std::clamp(value, 0.0f, 1.0f); }
        PROPERTY(Volume)

        void OnNotify() override;

        void SerializedReflectEvent() override;
        void DeserializedReflectEvent() override;
        void ImGuiDrawPropertysEvent() override;

    public:
        void SetAudioFromGuid(const File::Guid& guid);
        void SetAudioFromPath(const File::Path& path);

    protected:
        File::Guid _audioGuid = File::NULL_GUID;
        float      _volume = 1.0f;

        REFLECT_FIELDS_BEGIN(EventContext)
        int AudioAssetID = 0;
        REFLECT_FIELDS_END(AudioEventContext)
    };

}; // namespace Timeline