#pragma once

class Model;
class Animator;
class Animation;
class MeshRenderer;
class FBXConverter;
class EditorModelDetails : public EditorTool
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
    virtual void OnTickGui() override;
    virtual void OnStartGui() override;
    virtual void OnEndGui() override;

private:
    /* Begin 호출 전에 호출 */
    virtual void OnPreFrameBegin() override;

    /* Begin 호출 직후 호출 */
    virtual void OnPostFrameBegin() override;

    /* Begin 호출 후 클리핑 테스트를 통과한 후 호출 */
    virtual void OnFrameRender() override;
    virtual void OnFrameClipped() override;

    /* End 호출 후에 호출 */
    virtual void OnFrameEnd() override;

    /* 프레임이 포커싱 될 때 호출 (OnPostFrameBegin 후에 호출) */
    virtual void OnFrameFocusEnter() override;
    virtual void OnFrameFocusStay() override;
    virtual void OnFrameFocusExit() override;

    /* Popup창 호출 성공 시 호출 (OnPreFrameBegin 전에 호출) */
    virtual void OnFramePopupOpened() override;
   
private:
    static FBXConverter& GetFBXConverter();

private:
    void ImportModel();
    void ExportModel();
    void SaveModel();

private:
    Matrix                          _worldMatrix;
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

    // Light Property
    Vector3 _direction;
    Vector3 _color;
    Vector3 _ambient;
    float   _intensity;
};
