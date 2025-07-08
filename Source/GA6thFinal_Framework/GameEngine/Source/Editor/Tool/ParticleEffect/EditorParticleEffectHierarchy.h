#pragma once
class EditorParticleEffectHierarchy : public EditorTool
{

public:
    EditorParticleEffectHierarchy();
    virtual ~EditorParticleEffectHierarchy();


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

    void OnFramePopupOpened() override;

 //   void Serialize(std::string filepath);

	//void Deserialize(const std::string& filepath);

private:
    EditorParticleEffectDetails* _editorParticleEffectDetails;
    EditorParticleEffectViewer*  _editorParticleEffectViewer;
    class ParticleEffect*        _curEffect  = nullptr;
    class ParticleEmitter*       _curEmitter = nullptr;
    void                         ShowCurrentEffects();


    bool ctrlOflag = true;
    bool ctrlSflag = true;
};
