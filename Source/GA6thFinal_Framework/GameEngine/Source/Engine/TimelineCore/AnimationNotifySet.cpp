#include "pch.h"
#include "AnimationNotifySet.h"

using namespace std::filesystem;

bool AnimationNotifySet::NewFile(const File::Path& filePath)
{
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
}

void AnimationNotifySet::ClearTimeline()
{
    _timelineTable.clear();
}

void AnimationNotifySet::SetActiveTimeline(std::string_view animKey) 
{
    _activeTimeline = GetTimeline(animKey);
}

std::shared_ptr<TimelineSystem> AnimationNotifySet::GetActiveTimeline() const
{
    return _activeTimeline;
}

bool AnimationNotifySet::AddTimeline(std::string_view animKey) 
{
    bool hasTimeline = HasTimeline(animKey);
    if (false == hasTimeline)
    {
        _timelineTable[animKey.data()] = std::make_shared<TimelineSystem>();
        return true;
    }
    else // 이미 존재하는 타임라인이므로 추가하지 않음
    {   
        return false;
    }
}

bool AnimationNotifySet::RemoveTimeline(std::string_view animKey)
{
    auto it = _timelineTable.find(animKey.data());
    if (it != _timelineTable.end())
    {
        if (nullptr != it->second && it->second == _activeTimeline)
        {
            _activeTimeline = nullptr; // 현재 활성화된 타임라인이 제거되는 경우
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

std::shared_ptr<TimelineSystem> AnimationNotifySet::GetTimeline(std::string_view animKey) const
{
    auto it = _timelineTable.find(animKey.data());
    if (it != _timelineTable.end())
    {
        return it->second;
    }
    return nullptr;
}

void AnimationNotifySet::SerializedReflectEvent()
{
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
        auto timeline = std::make_shared<TimelineSystem>();
        timeline->DeserializedReflectFields(serializedData);
        _timelineTable[animKey] = timeline;
    }
}