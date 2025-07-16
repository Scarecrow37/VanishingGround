#pragma once

class EditorParticleEffectViewer : public EditorTool
{
public:
    EditorParticleEffectViewer();
    virtual ~EditorParticleEffectViewer();


public:
    void OnTickGui() override;

    void OnStartGui() override;

    void OnEndGui() override;

private:
    void OnPreFrameBegin() override;

    void OnPostFrameBegin() override;

    void OnFrameClipped() override;

    void OnFrameEnd() override;

    void OnFrameFocusEnter() override;

    void OnFrameFocusStay() override;

    void OnFrameFocusExit() override;

    void OnFrameRender() override;

    void OnFramePopupOpened() override;

protected:
    void SerializedReflectEvent() override;

    void DeserializedReflectEvent() override;



private:
    std::unique_ptr<EditorDynamicCamera> _camera;
    //EditorParticleEffectDetails*                  _editorParticleEffectDetails;
};
