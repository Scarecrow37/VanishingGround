#pragma once
#include "Note/QTENote.h"

namespace QTE
{
    // QTE에 사용하는 트랙
    // 타임라인 트랙, QTE 딜레이, 
    class Track : public ReflectSerializer
    {
    public:
        inline static const File::Path DEFAULT_NAME = L"QTE_Track.UmQTETrack";
        inline static const File::Path EXTENSION    = L".UmQTETrack";

    public:
        Track();
        ~Track();

    public:
        void Clear();

        bool IsLoadedFile() const;
        bool NewFile(const File::Path& filePath);
        bool SaveFile(const File::Path& filePath, bool overwrite = false);
        bool LoadFile(const File::Path& filePath);

        inline void  SetQTESpeedScale(float scale) { ReflectFields->QTESpeedScale = scale; }
        inline float GetQTESpeedScale() const { return ReflectFields->QTESpeedScale; }
        inline float GetMinFrame() const { return _eventTrack ? _eventTrack->GetMinFrame() : 0.0f; }
        inline float GetMaxFrame() const { return _eventTrack ? _eventTrack->GetMaxFrame() : 0.0f; }
        inline const File::Path& GetFilePath() const { return _filePath; }
        inline std::weak_ptr<Timeline::EventTrack> GetEventTrack() { return _eventTrack; }
    public:
        void SerializedReflectEvent() override;
        void DeserializedReflectEvent() override;

    private:
        File::Path _filePath = File::NULL_PATH;
        std::shared_ptr<Timeline::EventTrack> _eventTrack;
        REFLECT_FIELDS_BEGIN(ReflectSerializer)
        float       QTESpeedScale = 1.0f;       // QTE 속도 배율
        std::string TrackSerializeData = "";    // 트랙 직렬화 데이터
        REFLECT_FIELDS_END(Track)

    public:
        Track(const Track& rhs)
        {
            *ReflectFields = *rhs.ReflectFields;
            _filePath      = rhs._filePath;
            _eventTrack    = rhs._eventTrack;
        }
        Track(Track&& rhs) noexcept
        {
            *ReflectFields = std::move(*rhs.ReflectFields);
            _filePath      = std::move(rhs._filePath);
            _eventTrack    = std::move(rhs._eventTrack);
        }
        Track& operator=(const Track& rhs)
        {
            if (this == &rhs)
                return *this;

            *ReflectFields = *rhs.ReflectFields;
            _filePath = rhs._filePath;
            _eventTrack = rhs._eventTrack;
            return *this;
        }
        Track& operator=(Track&& rhs) noexcept
        {
            if (this == &rhs)
                return *this;

            *ReflectFields = std::move(*rhs.ReflectFields);
            _filePath      = std::move(rhs._filePath);
            _eventTrack    = std::move(rhs._eventTrack);
            return *this;
        }

    };
} // namespace QTE