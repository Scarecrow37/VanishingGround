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
        REFLECT_PROPERTY(AssetID, Path, Volume) 

        GETTER_ONLY(std::string, Path) { return UmFileSystem.GetPathFromAssetID(ReflectFields->AudioAssetID).string(); }
        PROPERTY(Path)

        GETTER(int, AssetID) { return ReflectFields->AudioAssetID; }
        SETTER(int, AssetID) { ReflectFields->AudioAssetID = value; }
        PROPERTY(AssetID)

        GETTER(float, Volume) { return ReflectFields->Volume; }
        SETTER(float, Volume) { ReflectFields->Volume = std::clamp(value, 0.0f, 1.0f); }
        PROPERTY(Volume)

        void OnNotify() override;

        void SerializedReflectEvent() override;
        void DeserializedReflectEvent() override;
        void ImGuiDrawPropertysEvent() override;

    public:
        void SetAudioFromGuid(const File::Guid& guid);
        void SetAudioFromPath(const File::Path& path);

    protected:
        File::Guid _guid = File::NULL_GUID;
        std::string _guidStr = "";
        REFLECT_FIELDS_BEGIN(EventContext)
        int AudioAssetID = 0;
        float Volume = 1.0f;
        REFLECT_FIELDS_END(AudioEventContext)
    };

}; // namespace Timeline