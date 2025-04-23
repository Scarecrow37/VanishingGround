#pragma once

struct EditorToolSerializeData
{
    std::string name                = "";
    bool        IsVisible           = false;
    bool        IsLock              = false;
    std::string ReflectionField     = "";
};

struct EditorSetting
{
    using ToolDataList = std::vector<EditorToolSerializeData>;

public:
    bool IsDebugMode;       // 에디터 디버그 모드 여부(에디터관련 정보 출력)

    ToolDataList ToolData;  // 툴 상태
};