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
    void Awake() override;
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
    std::string CurrentAnimationKey = "";
    float       AnimationSpeed     = 1.0f;
    bool        IsAnimationPlaying = false;
    bool        IsAnimationLooping = true;
    REFLECT_FIELDS_END(SkeletalMeshRenderer)

    /////////////////////////////////////////////////////////////
    /// Animation
    /////////////////////////////////////////////////////////////
public:
    void SetCurrentAnimation(std::string_view animKey);
    void SetAnimationLoop(bool looping);
    void SetAnimationFrame(float frame);
    void SetAnimationSpeed(float speed);
    void StopAnimation();
    void PlayAnimation();
    void PauseAnimation();
    void ResumeAnimation();
    bool IsAnimationEnd();

    inline float              GetCurrentAnimationTime()     const { return _animationTime; }
    inline const std::string& GetCurrentAnimationName()     const { return ReflectFields->CurrentAnimationKey; }
    inline float              GetCurrentAnimationSpeed()    const { return ReflectFields->AnimationSpeed; }
    inline bool               IsAnimationPlaying()          const { return ReflectFields->IsAnimationPlaying; }
    inline bool               IsAnimationLooping()          const { return ReflectFields->IsAnimationLooping; }
    

private:
    float _animationTime = 0.0f;

};