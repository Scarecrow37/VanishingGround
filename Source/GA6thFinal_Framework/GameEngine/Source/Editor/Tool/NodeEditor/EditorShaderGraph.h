#pragma once
#include "UmFramework.h"
#include "blueprint.h"

class EditorShaderGraph :
    public EditorTool
{
public:
    EditorShaderGraph();
    virtual ~EditorShaderGraph();
private:
    virtual void  OnStartGui() override;

    virtual void  OnPreFrame() override;

    virtual void  OnFrame() override;

    virtual void  OnPostFrame() override;

    blueprint _bluePrint;
};