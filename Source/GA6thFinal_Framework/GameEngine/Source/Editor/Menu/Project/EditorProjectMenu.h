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

/*
프로젝트 루트 설정
*/
class EditorMenuProjectRoot : public EditorMenu
{
public:
    EditorMenuProjectRoot()
    {
        SetCallOrder(0);
        SetPath("Project");
        SetLabel("ProjectManager");
    }
    virtual ~EditorMenuProjectRoot() = default;

public:
    virtual void OnMenu() override;
};