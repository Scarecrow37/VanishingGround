#pragma once

struct EditorToolSerializeData;

using ToolDataList = std::vector<EditorToolSerializeData>;

struct EditorToolSerializeData
{
    std::string  Class           = "";
    std::string  Name            = "";
    bool         IsVisible       = false;
    bool         IsLock          = false;
    std::string  ReflectionField = "";
    ToolDataList ChildToolData;
};

struct EditorSetting
{
    bool IsDebugMode;       // 에디터 디버그 모드 여부(에디터관련 정보 출력)

    ToolDataList ToolData;  // 툴 상태

    std::string ImGuiData;
};