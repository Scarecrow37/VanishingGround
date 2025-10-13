#pragma once

class ILight;
class IMeshRenderer;
class EditorParticleEffectHierarchy : public EditorTool
{

public:
    EditorParticleEffectHierarchy();
    virtual ~EditorParticleEffectHierarchy();


public:
    void OnStartGui() override;

private:
    void OnPostFrameBegin() override;

private:
    EditorParticleEffectDetails* _editorParticleEffectDetails;
    EditorParticleEffectViewer*  _editorParticleEffectViewer;
    class ParticleEffect*        _curEffect  = nullptr;
    class ParticleEmitter*       _curEmitter = nullptr;    

    File::Path currentmeshsurfacepath = File::NULL_PATH;
    bool       ctrlOflag              = true;
    bool       ctrlSflag              = true;

    Vector3                        _defaultpos = Vector3(0, 0, 0);
    GraphicsPointer<IMeshRenderer> _meshRenderer;
    bool                           _isModelActive = true;
    File::Path                     _envmodelpath  = File::NULL_PATH;
    Matrix                         _worldMatrix;
    Vector3                        _position    = Vector3(0, -20, 0);
    Vector3                        _rotation    = Vector3::Zero;
    Quaternion                     _quaternion  = Quaternion::Identity;
    Vector3                        _scale       = Vector3::One;
    bool                           _isDirtyFlag = false;


    void LoadEnvironmentModel(const File::Path& path);


    // Light Property
    Vector3                 _direction;
    Vector3                 _color;
    Vector3                 _ambient;
    float                   _intensity;
    bool                    _lightActivity;
    GraphicsPointer<ILight> _directionalLight;
};
