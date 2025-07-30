#pragma once

class AnimationNotifySet : public ReflectSerializer
{
public:
    AnimationNotifySet() = default;
    ~AnimationNotifySet() = default;

public:
    inline static const File::Path DEFAULT_NAME = L"NotifySet.UmAnimNotifySet";
    inline static const File::Path EXTENSION = L".UmAnimNotifySet";

public:
    void Clear();
    void ClearTimeline();

    bool IsLoadedFile() const;
    bool NewFile(const File::Path& filePath);
    bool SaveFile(const File::Path& filePath, bool overwrite = false);
    bool LoadFile(const File::Path& filePath);
    
    bool SetActiveTimeline(std::string_view animKey);
    std::shared_ptr<Timeline::EventTrack> GetActiveTimeline() const;
    const std::string& GetActiveTimelineName() const;
    bool AddTimeline(std::string_view animKey, bool active = false);
    bool ChangeTimelineName(std::string_view oldAnimKey, std::string_view newAnimKey);
    bool RemoveTimeline(std::string_view animKey);
    bool HasTimeline(std::string_view animKey) const;
    std::shared_ptr<Timeline::EventTrack> GetTimeline(std::string_view animKey) const;
    const std::map<std::string, std::shared_ptr<Timeline::EventTrack>>& GetTimelineTable() const;
    const File::Path& GetFilePath() const;

private:
    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

private:
    File::Path _filePath = File::NULL_PATH;
    std::pair<std::string, std::shared_ptr<Timeline::EventTrack>> _activeTimeline = {"", nullptr}; 
    std::map<std::string, std::shared_ptr<Timeline::EventTrack>> _timelineTable;
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    // 애니메이션 키 - 타임라인 시스템 매핑
    std::unordered_map<std::string, std::string> SerializeData;
    REFLECT_FIELDS_END(AnimationNotifySet)

};
