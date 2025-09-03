#pragma once
#include "Note/QTENote.h"

namespace QTE
{
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

        std::weak_ptr<Timeline::EventTrack> GetEventTrack() { return _eventTrack; }

    private:
        File::Path _filePath = File::NULL_PATH;
        std::shared_ptr<Timeline::EventTrack> _eventTrack;
        REFLECT_FIELDS_BEGIN(ReflectSerializer)
        std::string SerializeData;
        REFLECT_FIELDS_END(Track)
    };
} // namespace QTE