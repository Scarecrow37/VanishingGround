#pragma once

namespace Timeline
{
    /// <summary>
    /// AudioEvent는 기본적으로 루프와 그룹 설정을 지원하지 않습니다.
    /// 루프 설정을 하게되는 경우 해제의 책임을 지는 객체가 없기 때문입니다...
    /// </summary>
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

        inline float GetVolume() const { return ReflectFields->Volume; }

    public:
        bool AddAudioFromAssetID(int assetID);
        bool RemoveAudioFromAssetID(int assetID);

    private:
        void ListenAudioFileDragDropEvent();

    protected:
        float _totalWeight = 0.0f;

        REFLECT_FIELDS_BEGIN(EventContext)
        float                Volume = 1.0f;
        std::map<int, float> AudioDataTable; // AssetID - Weight
        REFLECT_FIELDS_END(AudioEventContext)

        inline static bool _isShowPath = false;
        inline static int _newAssetID;
    };

}; // namespace Timeline