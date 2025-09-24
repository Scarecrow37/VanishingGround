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
        REFLECT_PROPERTY(Volume) 

        GETTER(float, Volume) { return ReflectFields->Volume; }
        SETTER(float, Volume) { ReflectFields->Volume = std::clamp(value, 0.0f, 1.0f); }
        PROPERTY(Volume)

        void OnNotify() override;

        void SerializedReflectEvent() override;
        void DeserializedReflectEvent() override;
        void ImGuiDrawPropertysEvent() override;

    public:
        bool AddAudioFromAssetID(int assetID);
        bool RemoveAudioFromAssetID(int assetID);

    private:
        void ListenAudioFileDragDropEvent();

    protected:
        std::map<int, std::string> _audioGuidTable; // AssetID - GuidStr
        float _totalWeight = 0.0f;

        REFLECT_FIELDS_BEGIN(EventContext)
        float Volume = 1.0f;
        std::map<int, float> AudioDataTable; // AssetID - Weight
        REFLECT_FIELDS_END(AudioEventContext)

        inline static bool _isShowPath = false;
        inline static int _newAssetID;
    };

}; // namespace Timeline