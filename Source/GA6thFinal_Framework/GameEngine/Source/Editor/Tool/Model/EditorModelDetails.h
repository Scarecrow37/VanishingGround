#pragma once

class Model;
class Animator;
class Animation;
class MeshRenderer;
class FBXConverter;
class EditorModelTool;

class EditorModelDetails 
    : public EditorTool
    , public File::FileEventSubscriber
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
    std::shared_ptr<Animator>  GetAnimator() const;
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
     void OnEndGui() override;

private:
    /* Begin 호출 전에 호출 */
    void OnPreFrameBegin() override;

    /* Begin 호출 직후 호출 */
    void OnPostFrameBegin() override;

    /* Begin 호출 후 클리핑 테스트를 통과한 후 호출 */
    void OnFrameRender() override;
    void OnFrameClipped() override;

    /* End 호출 후에 호출 */
    void OnFrameEnd() override;

    /* 프레임이 포커싱 될 때 호출 (OnPostFrameBegin 후에 호출) */
    void OnFrameFocusEnter() override;
    void OnFrameFocusStay() override;
    void OnFrameFocusExit() override;

    /* Popup창 호출 성공 시 호출 (OnPreFrameBegin 전에 호출) */
    void OnFramePopupOpened() override;

    void OnRequestedDragDrop(const File::Path& path) override;
   
private:
    static FBXConverter& GetFBXConverter();

private:
    bool ImportModelWithDialog();
    void ImportModel(const File::Path& path);
    void ExportModel();
    void SaveModel();

private:
    EditorModelTool*                _modelTool = nullptr;
        
    Matrix                          _worldMatrix;
    Vector3                         _position = Vector3::Zero;
    Vector3                         _rotation = Vector3::Zero;
    Quaternion                      _quaternion = Quaternion::Identity;
    Vector3                         _scale = Vector3::One;

    std::filesystem::path           _filePath;
    std::shared_ptr<Animator>       _animator;
    std::unique_ptr<MeshRenderer>   _meshRenderer;
    std::unique_ptr<Light>          _mainLight;
    unsigned int                    _selectedMeshIndex = 0;

    // Animation Data
    size_t                          _currentAnimationIndex = 0;
    std::string                     _currentAnimationName  = "";
    AnimTable                       _animationIndexMap;
    float                           _animationSpeed = 1.0f;
    float                           _animationTime  = 0.0f;
    bool                            _isAnimationPlaying = false;
    bool                            _isAnimationLooping = true;

    bool _isModelActive = true;
    bool _isLightActive = true;

    // Light Property
    Vector3 _direction;
    Vector3 _color;
    Vector3 _ambient;
    float   _intensity;
};
