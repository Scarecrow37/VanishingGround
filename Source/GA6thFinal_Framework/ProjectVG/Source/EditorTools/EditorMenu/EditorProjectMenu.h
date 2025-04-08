#pragma once
#include "UmFramework.h"

/*
스크립트 빌드
*/
class EditorMenuScriptBuilder : public EditorMenuLeaf
{
public:
    EditorMenuScriptBuilder() {
        SetCallOrder(0);
        SetMenuPath("Project/Build");
        SetLabel("Script Build");
    }
    virtual ~EditorMenuScriptBuilder() = default;
public:
    virtual void OnSelected() override;
};