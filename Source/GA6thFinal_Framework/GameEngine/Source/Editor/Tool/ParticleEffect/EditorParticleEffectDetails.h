#pragma once
class EditorParticleEffectDetails : public EditorTool
{
public:
    EditorParticleEffectDetails();
    virtual ~EditorParticleEffectDetails();

    void OnTickGui() override;

    void OnStartGui() override;

    void OnEndGui() override;

    void SetCurrentEffect(class ParticleEffect* curEffect);
    void SetCurrentEmitter(class ParticleEmitter* curEmitter);

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

    void ProcessPopupFrame() override;

    void ProcessFocusFrame() override;

    void ProcessRenderFrame() override;

    void ShowEmitterDetails();
    void ShowEffectDetails();


    class ParticleEffect* _curEffect = nullptr;
    class ParticleEmitter* _curEmitter = nullptr;

};
