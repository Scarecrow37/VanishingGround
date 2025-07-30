#include "pch.h"
#include "AnimationNotifySet.h"

using namespace std::filesystem;

void AnimationNotifySet::Clear()
{
    _filePath = File::NULL_PATH;
    ClearTimeline();
}

void AnimationNotifySet::ClearTimeline()
{
    _timelineTable.clear();
    _activeTimeline = {"", nullptr};
}

bool AnimationNotifySet::IsLoadedFile() const
{
    return false == _filePath.IsNull();
}

bool AnimationNotifySet::NewFile(const File::Path& filePath)
{
    Clear();
    std::ofstream fout(filePath);
    if (fout.is_open())
    {
        SaveFile(filePath, true);
        fout.close();
    }
    return false;
}

bool AnimationNotifySet::SaveFile(const File::Path& filePath, bool overwrite)
{
    if (false == overwrite && false == exists(filePath))
    {
        return false;
    }
    std::ofstream fout(filePath);
    if (true == fout.is_open())
    {
        YAML::Node  node;
        node["TimelineTable"] = SerializedReflectFields();
        fout << node;
        fout.close();
        _filePath = filePath;
        return true;
    }
    return false;
}

bool AnimationNotifySet::LoadFile(const File::Path& filePath)
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
    if (node["TimelineTable"])
    {
        std::string SerialData = node["TimelineTable"].as<std::string>();
        DeserializedReflectFields(SerialData);
        _filePath = filePath;
    }
    return true;
}

bool AnimationNotifySet::SetActiveTimeline(std::string_view animKey)
{
    auto timeline = GetTimeline(animKey);
    if (nullptr != timeline && _activeTimeline.second != timeline)
    {
        _activeTimeline = {animKey.data(), timeline};
        return true;
    }
    return false;
}

std::shared_ptr<Timeline::EventTrack> AnimationNotifySet::GetActiveTimeline() const
{
    return _activeTimeline.second;
}

const std::string& AnimationNotifySet::GetActiveTimelineName() const
{
    return _activeTimeline.first;
}

bool AnimationNotifySet::AddTimeline(std::string_view animKey, bool active)
{
    bool hasTimeline = HasTimeline(animKey);
    if (false == hasTimeline)
    {
        _timelineTable[animKey.data()] = std::make_shared<Timeline::EventTrack>();
        if (true == active)
        {
            SetActiveTimeline(animKey); // 활성화된 타임라인 설정
        }
        return true;
    }
    else // 이미 존재하는 타임라인이므로 추가하지 않음
    {   
        return false;
    }
}

bool AnimationNotifySet::ChangeTimelineName(std::string_view oldAnimKey, std::string_view newAnimKey)
{
    auto it = _timelineTable.find(oldAnimKey.data());
    if (it != _timelineTable.end())
    {
        if (HasTimeline(newAnimKey))
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

bool AnimationNotifySet::RemoveTimeline(std::string_view animKey)
{
    auto it = _timelineTable.find(animKey.data());
    if (it != _timelineTable.end())
    {
        if (nullptr != it->second && it->second == _activeTimeline.second)
        {
            _activeTimeline.first  = "";      // 현재 활성화된 타임라인이 제거되는 경우
            _activeTimeline.second = nullptr; // 현재 활성화된 타임라인이 제거되는 경우
        }
        _timelineTable.erase(it);
        return true;
    }
    else // 해당 애니메이션 키에 대한 타임라인이 존재하지 않음
    {
        return false;
    }
}

bool AnimationNotifySet::HasTimeline(std::string_view animKey) const
{
    auto it = _timelineTable.find(animKey.data());
    if (it != _timelineTable.end())
    {
        return true;
    }
    return false;
}

std::shared_ptr<Timeline::EventTrack> AnimationNotifySet::GetTimeline(std::string_view animKey) const
{
    auto it = _timelineTable.find(animKey.data());
    if (it != _timelineTable.end())
    {
        return it->second;
    }
    return nullptr;
}

const std::map<std::string, std::shared_ptr<Timeline::EventTrack>>& AnimationNotifySet::GetTimelineTable() const
{
    return _timelineTable;
}

const File::Path& AnimationNotifySet::GetFilePath() const
{
    return _filePath;
}

void AnimationNotifySet::SerializedReflectEvent()
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

void AnimationNotifySet::DeserializedReflectEvent() 
{
    ClearTimeline();
    for (const auto& [animKey, serializedData] : ReflectFields->SerializeData)
    {
        auto timeline = std::make_shared<Timeline::EventTrack>();
        timeline->DeserializedReflectFields(serializedData);
        _timelineTable[animKey] = timeline;
    }
}