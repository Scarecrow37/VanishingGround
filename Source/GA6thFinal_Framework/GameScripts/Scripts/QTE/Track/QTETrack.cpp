#include "pchScripts.h"
#include "QTETrack.h"
#include <WeaponSystem/WeaponSystem.h>

namespace QTE
{
    REFLECT_FUNCTION(Track)

    Track::Track() 
        : _eventTrack(std::make_shared<Timeline::EventTrack>())
    {
    }

    Track::~Track() = default;

    void Track::Clear()
    {
        _filePath = File::NULL_PATH;
        if (_eventTrack)
        {
            _eventTrack->Clear();
        }
    }

    bool Track::IsLoadedFile() const
    {
        return false == _filePath.IsNull();
    }

    bool Track::NewFile(const File::Path& filePath)
    {
        Clear();
        return SaveFile(filePath, true);
    }

    bool Track::SaveFile(const File::Path& filePath, bool overwrite)
    {
        if (false == overwrite && true == exists(filePath))
        {
            return false;
        }
        const File::Path& path = filePath == File::NULL_PATH ? GetFilePath() : filePath;
        std::ofstream     fout(path);
        if (true == fout.is_open())
        {
            YAML::Node node;
            node["SerializeData"] = SerializedReflectFields();
            fout << node;
            fout.close();
            _filePath = path;
            return true;
        }
        return false;
    }

    bool Track::LoadFile(const File::Path& filePath)
    {
        Clear();
        if (false == exists(filePath))
        {
            return false;
        }
        YAML::Node node = YAML::LoadFile(filePath.string());
        if (true == node.IsNull())
        {
            return false;
        }
        if (node["SerializeData"])
        {
            std::string SerialData = node["SerializeData"].as<std::string>();
            DeserializedReflectFields(SerialData);
            _filePath = filePath;
        }
        return true;
    }

    void Track::SerializedReflectEvent() 
    {
        if (_eventTrack)
        {
            ReflectFields->TrackSerializeData = _eventTrack->SerializedReflectFields();
        }
    }

    void Track::DeserializedReflectEvent() 
    {
        if (_eventTrack)
        {
            _eventTrack->DeserializedReflectFields(ReflectFields->TrackSerializeData);
        }
    }
}