#pragma once
#include "MeshComponent.h"
#include "Animation/Structs/AnimationStructs.h"

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
    AnimationData* GetLastAnimationDataEx();
    
    void SetAnimation(AnimationData* animData, std::string_view animKey, bool blend);
    void SetAnimationLoop(AnimationData* animData, bool loop);
    void SetAnimationFrame(AnimationData* animData,float frame);
    void SetAnimationSpeed(AnimationData* animData,float speed);

public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return _guidRef.ToPath().string(); }
    PROPERTY(FilePath)

private:
    File::GuidRef _guidRef;

    REFLECT_FIELDS_BEGIN(MeshComponent)
    std::string Guid                    = "";
    std::string MainAnimationKey        = "";
    bool        MainAnimationLooping    = true;
    bool        MainAnimationSpeed      = true;
    float       AnimationSpeedScale     = 1.0f;
    REFLECT_FIELDS_END(SkeletalMeshRenderer)

    /////////////////////////////////////////////////////////////
    /// Animation
    /////////////////////////////////////////////////////////////
public:
    /// <summary>
    /// 애니메이션 오버라이드를 삽입합니다.
    /// </summary>
    /// <param name="animKey"></param>
    /// <param name="loop"></param>
    /// <param name="blend"></param>
    /// <param name="popCondition"></param>
    void PushOverrideAnimation(std::string_view animKey, bool loop = true, bool blend = true,
                               std::function<bool(const AnimationData&)> popCondition = nullptr);

    /// <summary>
    /// 애니메이션 오버라이드를 뺍니다
    /// </summary>
    /// <param name="blend"></param>
    void PopOverrideAnimation(bool blend = true);

    /// <summary>
    /// 현재 애니메이션을 바꿉니다.
    /// </summary>
    /// <param name="animKey">애니메이션 키</param>
    /// <param name="loop">루프 여부. 기본 값은 true입니다.</param>
    void SetCurrentAnimation(std::string_view animKey, bool blend = true);
    void SetMainAnimation(std::string_view animKey, bool blend = true);

    /// <summary>애니메이션의 루프 여부를 설정합니다.</summary>
    /// <param name="loop">애니메이션 루프 여부</param>
    void SetCurrentAnimationLoop(bool loop);
    void SetMainAnimationLoop(bool loop);

    /// <summary>애니메이션의 프레임을 설정합니다.</summary>
    /// <param name="frame">애니메이션 프레임 수</param>
    void SetCurrentAnimationFrame(float frame);
    void SetMainAnimationFrame(float frame);

    /// <summary>애니메이션의 스피드를 설정합니다.</summary>
    /// <param name="frame">애니메이션 스피드</param>
    void SetCurrentAnimationSpeed(float speed);
    void SetMainAnimationSpeed(float speed);

    /// <summary>애니메이션을 0프레임으로 맞춘 후 멈춥니다.</summary>
    void StopCurrentAnimation();

    /// <summary>애니메이션을 0프레임으로 맞춘 후 재생합니다.</summary>
    void PlayCurrentAnimation();

    /// <summary>애니메이션을 멈춥니다.</summary>
    void PauseCurrentAnimation();

    /// <summary>애니메이션을 재생합니다.</summary>
    void ResumeCurrentAnimation();

    const AnimationData& GetMainAnimationData() const;
    const AnimationData& GetLastAnimationData() const;

    inline const std::string& GetCurrentAnimationName()     const { return GetLastAnimationData().AnimationName; }
    inline float              GetCurrentAnimationTime()     const { return GetLastAnimationData().Duration; }
    inline float              GetCurrentAnimationSpeed()    const { return GetLastAnimationData().Speed; }
    inline bool               IsAnimationLooping()          const { return GetLastAnimationData().IsLooping; }
    inline bool               IsAnimationPlaying()          const { return GetLastAnimationData().IsPlaying; }

private:
    AnimationData              _mainAnimationData;
    std::vector<AnimationData> _overrideAnimationStack; 
};