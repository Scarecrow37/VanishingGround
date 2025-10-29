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
    void OnFrameRender() override;

private:
    EditorParticleEffectDetails* _editorParticleEffectDetails;
    EditorParticleEffectViewer*  _editorParticleEffectViewer;
    class ParticleEffect*        _curEffect  = nullptr;
    class ParticleEmitter*       _curEmitter = nullptr;    

    File::Path _currentMeshSurfaceModelPath = File::NULL_PATH;
    bool       _ctrlOflag              = true;
    bool       _ctrlSflag              = true;

    GraphicsPointer<IMeshRenderer> _meshRenderer;
	bool                           _hideModelTransform = false;
    bool                           _isModelActive = true;
    File::Path                     _envModelPath  = File::NULL_PATH;
    Matrix                         _worldMatrix;
    Vector3                        _position    = Vector3(0, -20, 0);
    Vector3                        _rotation    = Vector3::Zero;
    Quaternion                     _quaternion  = Quaternion::Identity;
    Vector3                        _scale       = Vector3::One;
    bool                           _isDirtyFlag = false;

    void LoadEnvironmentModel(const File::Path& path);
    void          LoadEffect();
    FBXConverter& GetFBXConverter();

    // Light Property
    Vector3                 _direction;
    Vector3                 _color;
    Vector3                 _ambient;
    float                   _intensity;
    bool                    _lightActivity;
    GraphicsPointer<ILight> _directionalLight;

    Matrix  _effectWorldMatrix = Matrix::Identity;
    Vector3 _effectPosition      = Vector3::Zero;
    Vector3 _effectRotation      = Vector3::Zero;
    Vector3 _effectScale         = {1, 1, 1};
    bool    _effectTransformHide = false;

    bool    _effectRotateFlag = false;
    float   _elapsedTimer     = 0.f;
    float   _rotationSpeed     = 1.f;
    Vector3 _rotationVelocity = Vector3::Zero;
    
    std::string _currentEffectFilePath = "";
    bool _boneFlag = false;
};
