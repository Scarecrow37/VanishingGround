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

    virtual void OnPreFrame() override;

    virtual void OnFrame() override;

    virtual void OnPostFrame() override;

    virtual void OnPopup() override;

    void UpdateFPS();
    float _elpasedTime;
    int   _frameCount;
    int   _fps;
};

