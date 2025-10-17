#pragma once
class EditorParticleEffectDetails : public EditorTool
{
public:
    EditorParticleEffectDetails();
    virtual ~EditorParticleEffectDetails();

    void SetCurrentEffect(class ParticleEffect* curEffect);
    void SetCurrentEmitter(class ParticleEmitter* curEmitter);

private:
    void OnFrameRender() override;
    void ShowEmitterDetails();
    void ShowEffectDetails();

private:
    class ParticleEffect*  _curEffect  = nullptr;
    class ParticleEmitter* _curEmitter = nullptr;
};
