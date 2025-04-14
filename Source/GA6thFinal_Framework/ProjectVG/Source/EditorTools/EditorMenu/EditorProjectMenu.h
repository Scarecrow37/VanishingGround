#pragma once
#include "UmFramework.h"

/*
스크립트 빌드
*/
class EditorMenuScriptBuilder : public EditorMenu
{
public:
    EditorMenuScriptBuilder() {
        SetCallOrder(0);
        SetLabel("Project/Build/Script Build");
    }
    virtual ~EditorMenuScriptBuilder() = default;
public:
    virtual void OnMenu() override;
};