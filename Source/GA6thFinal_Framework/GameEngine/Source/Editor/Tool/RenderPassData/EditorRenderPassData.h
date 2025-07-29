#pragma once

class EditorRenderPassData : public EditorTool
{
public:
    EditorRenderPassData();
    virtual ~EditorRenderPassData();

private:
    void OnFrameRender() override;
};
