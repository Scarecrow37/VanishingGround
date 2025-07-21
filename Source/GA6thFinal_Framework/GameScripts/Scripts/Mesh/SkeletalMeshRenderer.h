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
    /// <summary>
    /// 현재 애니메이션을 바꿉니다.
    /// </summary>
    /// <param name="animKey">애니메이션 키</param>
    /// <param name="loop">루프 여부. 기본 값은 true입니다.</param>
    void SetCurrentAnimation(std::string_view animKey, bool loop = true);

    /// <summary>애니메이션의 루프 여부를 설정합니다.</summary>
    /// <param name="loop">애니메이션 루프 여부</param>
    void SetAnimationLoop(bool loop);

    /// <summary>애니메이션의 프레임을 설정합니다.</summary>
    /// <param name="frame">애니메이션 프레임 수</param>
    void SetAnimationFrame(float frame);

    /// <summary>애니메이션의 스피드를 설정합니다.</summary>
    /// <param name="frame">애니메이션 스피드</param>
    void SetAnimationSpeed(float speed);

    /// <summary>애니메이션을 0프레임으로 맞춘 후 멈춥니다.</summary>
    void StopAnimation();

    /// <summary>애니메이션을 0프레임으로 맞춘 후 재생합니다.</summary>
    void PlayAnimation();

    /// <summary>애니메이션을 멈춥니다.</summary>
    void PauseAnimation();

    /// <summary>애니메이션을 재생합니다.</summary>
    void ResumeAnimation();

    /// <summary>애니메이션의 종료 여부를 반환합니다.</summary>
    /// <returns>애니메이션 종료 여부. 루프 시에는 항상 false를 반환합니다.</returns>
    bool IsAnimationEnd();

    inline float              GetCurrentAnimationTime()     const { return _animationTime; }
    inline const std::string& GetCurrentAnimationName()     const { return ReflectFields->CurrentAnimationKey; }
    inline float              GetCurrentAnimationSpeed()    const { return ReflectFields->AnimationSpeed; }
    inline bool               IsAnimationPlaying()          const { return ReflectFields->IsAnimationPlaying; }
    inline bool               IsAnimationLooping()          const { return ReflectFields->IsAnimationLooping; }
    
    // Anim Override 추가?
private:
    float _animationTime = 0.0f;

};