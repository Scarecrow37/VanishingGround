#pragma once

class EditorParticleEffectViewer : public EditorTool
{
public:
    EditorParticleEffectViewer();
    virtual ~EditorParticleEffectViewer();

public:
    void OnStartGui() override;

private:
    void OnPostFrameBegin() override;

    void OnFrameRender() override;

private:
    std::unique_ptr<EditorDynamicCamera> _camera;
};
