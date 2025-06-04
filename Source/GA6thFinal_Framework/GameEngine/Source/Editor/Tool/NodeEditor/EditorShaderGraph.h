#pragma once

class blueprint;

class EditorShaderGraph :
    public EditorTool
{
public:
    EditorShaderGraph();
    virtual ~EditorShaderGraph();

private:
    virtual void OnStartGui() override;
    virtual void OnEndGui() override;

    virtual void OnFrameRender() override;

    blueprint* _bluePrint;
};