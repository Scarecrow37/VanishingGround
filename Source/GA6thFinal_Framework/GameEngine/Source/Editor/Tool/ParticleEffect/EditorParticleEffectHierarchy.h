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

private:
    EditorParticleEffectDetails* _editorParticleEffectDetails;
    EditorParticleEffectViewer*  _editorParticleEffectViewer;
    class ParticleEffect*        _curEffect  = nullptr;
    class ParticleEmitter*       _curEmitter = nullptr;
    void                         ShowCurrentEffects();

    File::Path currentmeshsurfacepath = File::NULL_PATH;
    bool       ctrlOflag              = true;
    bool       ctrlSflag              = true;

    Vector3                       defaultpos = Vector3(0, 0, 0);
    std::unique_ptr<MeshRenderer> _meshRenderer;
    bool                          _isModelActive = true;
    File::Path                    _envmodelpath  = File::NULL_PATH;
    Matrix                        _worldMatrix;
    Vector3                       _position   = Vector3(0,-20,0);
    Vector3                       _rotation   = Vector3::Zero;
    Quaternion                    _quaternion = Quaternion::Identity;
    Vector3                       _scale      = Vector3::One;
    bool                          _isDirtyFlag = false;


    void LoadEnvironmentModel(const File::Path& path);

    FBXConverter& GetFBXConverter();


    // Light Property
    Vector3      _direction;
    Vector3      _color;
    Vector3      _ambient;
    float        _intensity;
    bool         _lightActivity;
    class Light* _directionalLight;
};
