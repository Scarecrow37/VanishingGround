#pragma once

class EditorDebugTool
    : public EditorTool
{
public:
    EditorDebugTool();
    virtual ~EditorDebugTool();
private:
    virtual void OnTickGui() override;

    virtual void OnStartGui() override;

    virtual void OnPreFrameBegin() override;

    virtual void OnPostFrameBegin() override;

    virtual void OnFrameRender() override;

    virtual void OnFrameEnd() override;

    virtual void OnFramePopupOpened() override;

    void UpdateFPS();

private:
    float _elpasedTime;
    int   _frameCount;
    int   _fps;
};

