#pragma once

struct EditorToolStatus
{
    std::string name        = "";
    bool        IsVisible   = false;
    bool        IsLock      = false;
};

struct EditorSetting
{
    bool                          IsDebugMode;  // 에디터 디버그 모드 여부(에디터관련 정보 출력)

    std::vector<EditorToolStatus> ToolStatus;   // 툴 상태
};