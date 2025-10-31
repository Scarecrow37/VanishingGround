#pragma once

class Model;
class Animation;
class IAnimator;
class IMeshRenderer;
class FBXConverter;
class EditorModelTool;

class EditorModelDetails : public EditorTool, public File::FileEventSubscriber
{
    using AnimTable = std::unordered_map<std::string, unsigned int>;
    friend class EditorModelTool;
    friend class EditorModelMenu;
    friend class EditorModelHierarchy;

public:
    EditorModelDetails();
    virtual ~EditorModelDetails() = default;

public:
    void SetSelectedMesh(unsigned int index) { _selectedMeshIndex = index; }

    std::shared_ptr<Model>     GetModel() const;
    IAnimator*                 GetAnimator() const;
    std::shared_ptr<Animation> GetAnimation() const;
    const std::string&         GetCurrentAnimationName() const;

    void ChangeAnimation(std::string_view anim);
    void SetCurrentAnimationSpeed(float speed);
    void SetCurrentAnimationTime(float time);
    void PlayCurrentAnimation();
    void ResumeCurrentAnimation();
    void PauseCurrentAnimation();
    void StopCurrentAnimation();
    
private:
    void UpdateModelTransform();

private:
     void OnTickGui() override;
     void OnStartGui() override;

private:
    void OnFrameRender() override;

    void OnRequestedDragDrop(const File::Path& path) override;
   
private:
    static FBXConverter& GetFBXConverter();

private:
    bool ImportModelWithDialog();
    void ImportModel(const File::Path& path);
    void ExportModel();
    void SaveModel();

private:
    EditorModelTool*      _modelTool = nullptr;
    Matrix                _worldMatrix;
    Vector3               _position   = Vector3::Zero;
    Vector3               _rotation   = Vector3::Zero;
    Quaternion            _quaternion = Quaternion::Identity;
    Vector3               _scale      = Vector3::One;
    
    std::filesystem::path          _filePath;
    IAnimator*                     _animator;
    GraphicsPointer<IMeshRenderer> _meshRenderer;
    GraphicsPointer<ILight>        _mainLight;
    unsigned int                   _selectedMeshIndex = 0;
    
    size_t                _currentAnimationIndex = 0;
    std::string           _currentAnimationName;
    AnimTable             _animationIndexMap;
    float                 _animationSpeed     = 1.0f;
    float                 _animationTime      = 0.0f;
    bool                  _isAnimationPlaying = false;
    bool                  _isAnimationLooping = true;

    bool _isModelActive = true;
    bool _isLightActive = true;

    // Light Property
    Vector3 _direction;
    Vector3 _color;
    Vector3 _ambient;
    float   _intensity;
};