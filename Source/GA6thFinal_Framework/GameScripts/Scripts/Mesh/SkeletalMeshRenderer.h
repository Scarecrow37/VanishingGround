#pragma once
#include "MeshComponent.h"

class SkeletalMeshRenderer : public MeshComponent
{
    USING_PROPERTY(SkeletalMeshRenderer)

public:
    SkeletalMeshRenderer();
    virtual ~SkeletalMeshRenderer() override;

public:

protected:
    void Reset() override;
    void Update() override;
    void OnDrawDebug() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

private:
    void LoadModel();
    void UpdateAnimation();

public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return _guidRef.ToPath().string(); }
    PROPERTY(FilePath)

private:
    File::GuidRef _guidRef;

    REFLECT_FIELDS_BEGIN(MeshComponent)
    std::string Guid;
    REFLECT_FIELDS_END(SkeletalMeshRenderer)

    /////////////////////////////////////////////////////////////
    /// Animation
    /////////////////////////////////////////////////////////////
public:
    void SetCurrentAnimation(std::string_view animKey);
    void SetAnimationFrame(float frame);
    void SetAnimationSpeed(float speed);
    void StopAnimation();
    void PlayAnimation();
    void PauseAnimation();
    void ResumeAnimation();

    inline const std::string& GetCurrentAnimationName()     const { return _currentAnimationKey; }
    inline float              GetCurrentAnimationTime()     const { return _animationTime; }
    inline float              GetCurrentAnimationSpeed()    const { return _animationSpeed; }
    inline bool               IsAnimationPlaying()          const { return _isAnimationPlaying; }
    inline bool               IsAnimationLooping()          const { return _isAnimationLooping; }

private:
    float       _animationSpeed      = 1.0f;
    float       _animationTime       = 0.0f;
    bool        _isAnimationPlaying  = false;
    bool        _isAnimationLooping  = true;
    std::string _currentAnimationKey = "";

};