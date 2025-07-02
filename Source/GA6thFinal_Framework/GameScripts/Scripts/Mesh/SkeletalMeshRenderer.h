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

private:
    bool _isAnimationPlaying = false;
    bool _isAnimationLooping = true;
    float _animationTime  = 0.0f;
    float _animationSpeed = 1.0f;
    std::string _currentAnimationKey = "";

};