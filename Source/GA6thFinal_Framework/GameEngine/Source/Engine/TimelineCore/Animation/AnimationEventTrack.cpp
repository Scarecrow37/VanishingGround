#include "pch.h"
#include "AnimationEventTrack.h"

REFLECT_FUNCTION(AnimationEventTrack)

using namespace std::filesystem;

void AnimationEventTrack::Clear()
{
    _filePath = File::NULL_PATH;
    ClearEventTrack();
}

void AnimationEventTrack::ClearEventTrack()
{
    _timelineTable.clear();
    _activeEventTrack = {"", nullptr};
}

bool AnimationEventTrack::IsLoadedFile() const
{
    return false == _filePath.IsNull();
}

bool AnimationEventTrack::NewFile(const File::Path& filePath)
{
    Clear();
    return SaveFile(filePath, true);
}

bool AnimationEventTrack::SaveFile(const File::Path& filePath, bool overwrite)
{
    if (false == overwrite && true == exists(filePath))
    {
        return false;
    }
    std::ofstream fout(filePath);
    if (fout.is_open())
    {
        YAML::Node  node;
        node["EventTrackTable"] = SerializedReflectFields();
        fout << node;
        fout.close();
        _filePath = filePath;
        return true;
    }
    return false;
}

bool AnimationEventTrack::LoadFile(const File::Path& filePath)
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
    if (node["EventTrackTable"])
    {
        std::string SerialData = node["EventTrackTable"].as<std::string>();
        DeserializedReflectFields(SerialData);
        _filePath = filePath;
    }
    return true;
}

bool AnimationEventTrack::SetActiveEventTrack(std::string_view animKey)
{
    auto timeline = GetEventTrack(animKey);
    if (nullptr != timeline && _activeEventTrack.second != timeline)
    {
        _activeEventTrack = {animKey.data(), timeline};
        return true;
    }
    return false;
}

std::shared_ptr<Timeline::EventTrack> AnimationEventTrack::GetActiveEventTrack() const
{
    return _activeEventTrack.second;
}

const std::string& AnimationEventTrack::GetActiveEventTrackName() const
{
    return _activeEventTrack.first;
}

bool AnimationEventTrack::AddEventTrack(std::string_view animKey, bool active)
{
    bool hasEventTrack = HasEventTrack(animKey);
    if (false == hasEventTrack)
    {
        _timelineTable[animKey.data()] = std::make_shared<Timeline::EventTrack>();
        if (true == active)
        {
            SetActiveEventTrack(animKey); // 활성화된 타임라인 설정
        }
        return true;
    }
    else // 이미 존재하는 타임라인이므로 추가하지 않음
    {   
        return false;
    }
}

bool AnimationEventTrack::ChangeEventTrackName(std::string_view oldAnimKey, std::string_view newAnimKey)
{
    auto it = _timelineTable.find(oldAnimKey.data());
    if (it != _timelineTable.end())
    {
        if (HasEventTrack(newAnimKey))
        {
            return false; // 새 이름으로 타임라인이 이미 존재하는 경우
        }
        auto timeline = it->second;
        _timelineTable.erase(it);
        _timelineTable[newAnimKey.data()] = timeline; // 새 이름으로 타임라인을 추가
        return true;
    }
    return false;
}

bool AnimationEventTrack::RemoveEventTrack(std::string_view animKey)
{
    auto it = _timelineTable.find(animKey.data());
    if (it != _timelineTable.end())
    {
        if (nullptr != it->second && it->second == _activeEventTrack.second)
        {
            _activeEventTrack.first  = "";      // 현재 활성화된 타임라인이 제거되는 경우
            _activeEventTrack.second = nullptr; // 현재 활성화된 타임라인이 제거되는 경우
        }
        _timelineTable.erase(it);
        return true;
    }
    else // 해당 애니메이션 키에 대한 타임라인이 존재하지 않음
    {
        return false;
    }
}

bool AnimationEventTrack::HasEventTrack(std::string_view animKey) const
{
    auto it = _timelineTable.find(animKey.data());
    if (it != _timelineTable.end())
    {
        return true;
    }
    return false;
}

std::shared_ptr<Timeline::EventTrack> AnimationEventTrack::GetEventTrack(std::string_view animKey) const
{
    auto it = _timelineTable.find(animKey.data());
    if (it != _timelineTable.end())
    {
        return it->second;
    }
    return nullptr;
}

const std::map<std::string, std::shared_ptr<Timeline::EventTrack>>& AnimationEventTrack::GetEventTrackTable() const
{
    return _timelineTable;
}

const File::Path& AnimationEventTrack::GetFilePath() const
{
    return _filePath;
}

void AnimationEventTrack::SerializedReflectEvent()
{
    ReflectFields->SerializeData.clear();
    for (const auto& [animKey, timeline] : _timelineTable)
    {
        if (nullptr != timeline)
        {
            ReflectFields->SerializeData[animKey] = timeline->SerializedReflectFields();
        }
    }
}

void AnimationEventTrack::DeserializedReflectEvent() 
{
    ClearEventTrack();
    for (const auto& [animKey, serializedData] : ReflectFields->SerializeData)
    {
        auto timeline = std::make_shared<Timeline::EventTrack>();
        timeline->DeserializedReflectFields(serializedData);
        _timelineTable[animKey] = timeline;
    }
}