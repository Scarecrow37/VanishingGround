#pragma once

class AnimationEventTrack : public ReflectSerializer
{
public:
    AnimationEventTrack() = default;
    ~AnimationEventTrack() = default;

public:
    inline static const File::Path DEFAULT_NAME = L"Animation_Event_Track.UmAnimEvent";
    inline static const File::Path EXTENSION = L".UmAnimEvent";

public:
    void Clear();
    void ClearEventTrack();

    bool IsLoadedFile() const;
    bool NewFile(const File::Path& filePath);
    bool SaveFile(const File::Path& filePath, bool overwrite = false);
    bool LoadFile(const File::Path& filePath);
    
    bool SetActiveEventTrack(std::string_view animKey);
    bool AddEventTrack(std::string_view animKey, bool active = false);
    bool ChangeEventTrackName(std::string_view oldAnimKey, std::string_view newAnimKey);
    bool RemoveEventTrack(std::string_view animKey);
    bool HasEventTrack(std::string_view animKey) const;
    const std::string& GetActiveEventTrackName() const;
    std::shared_ptr<Timeline::EventTrack> GetActiveEventTrack() const;
    std::shared_ptr<Timeline::EventTrack> GetEventTrack(std::string_view animKey) const;
    const std::map<std::string, std::shared_ptr<Timeline::EventTrack>>& GetEventTrackTable() const;
    const File::Path& GetFilePath() const;

private:
    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

private:
    File::Path _filePath = File::NULL_PATH;
    std::pair<std::string, std::shared_ptr<Timeline::EventTrack>> _activeEventTrack = {"", nullptr}; 
    std::map<std::string, std::shared_ptr<Timeline::EventTrack>> _timelineTable;
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    // 애니메이션 키 - 타임라인 시스템 매핑
    std::unordered_map<std::string, std::string> SerializeData;
    REFLECT_FIELDS_END(AnimationEventTrack)

};
