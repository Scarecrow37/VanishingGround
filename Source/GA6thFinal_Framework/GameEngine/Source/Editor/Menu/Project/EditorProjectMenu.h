#pragma once

/*
스크립트 빌드
*/
class EditorMenuScriptBuilder : public EditorMenu
{
public:
    EditorMenuScriptBuilder() {
        SetCallOrder(0);
        SetPath("Project/Build");
        SetLabel("");
    }
    virtual ~EditorMenuScriptBuilder() = default;
public:
    virtual void OnMenu() override;
};