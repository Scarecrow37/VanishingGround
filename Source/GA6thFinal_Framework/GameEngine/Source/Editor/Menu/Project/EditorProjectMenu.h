#pragma once

/*
스크립트 빌드
*/
class EditorMenuScriptBuilder : public EditorMenu
{
public:
    EditorMenuScriptBuilder() 
    {
        SetPath("Project/Build");
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
        SetPath("Project");
    }
    virtual ~EditorMenuProjectRoot() = default;

public:
    virtual void OnMenu() override;
};