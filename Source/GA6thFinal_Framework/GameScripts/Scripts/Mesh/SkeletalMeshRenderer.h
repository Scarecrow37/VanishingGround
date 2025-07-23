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
    void UpdateAnimation(AnimationData* animData);
    AnimationData* GetLastAnimationDataEx();
    
    void SetAnimation(AnimationData* animData, std::string_view animKey, bool blend);
    void SetAnimationFrame(AnimationData* animData,float frame);
    void SetAnimationFlags(AnimationData* animData, int flags);

public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return _guidRef.ToPath().string(); }
    PROPERTY(FilePath)

private:
    File::GuidRef _guidRef;

    REFLECT_FIELDS_BEGIN(MeshComponent)
    std::string Guid                    = "";
    std::string MainAnimationKey        = "";
    int         MainAnimationFlags      = ANIMATION_FLAG_NONE;
    bool        MainAnimationSpeed      = true;
    float       AnimationSpeedScale     = 1.0f;
    REFLECT_FIELDS_END(SkeletalMeshRenderer)

    /////////////////////////////////////////////////////////////
    /// Animation
    /////////////////////////////////////////////////////////////
public:
    /// <summary>
    /// 애니메이션 오버라이드를 모두 지웁니다.
    /// </summary>
    void ClearOverrideAnimations();

    /// <summary>
    /// <para>오버라이드 애니메이션 빌드를 시작합니다.</para>
    /// <para>빌드 중에는 애니메이션이 바뀌지 않고, End를 호출할 때 마지막 애니메이션으로 바꿉니다.</para>
    /// <para>Animation Blend의 혼동을 방지하기 위해 사용합니다.</para>
    /// </summary>
    void BeginBuildOverrideAnimation();

    /// <summary>
    /// <para>오버라이드 애니메이션 빌드를 끝냅니다.</para>
    /// <para>빌드 종료 시점에서의 마지막 애니메이션으로 바꿉니다.</para>
    /// </summary>
    void EndBuildOverrideAnimation();

    /// <summary>애니메이션 오버라이드를 삽입합니다.</summary>
    /// <param name="animKey"></param>
    /// <param name="blend"></param>
    /// <param name="popCondition"></param>
    void PushOverrideAnimation(std::string_view animKey, bool blend = true, std::function<bool(const AnimationData&)> popCondition = nullptr);

    /// <summary>애니메이션 오버라이드를 뺍니다.</summary>
    void PopOverrideAnimation();

    /// <summary>현재 애니메이션을 바꿉니다.</summary>
    /// <param name="animKey">애니메이션 키</param>
    /// <param name="loop">루프 여부. 기본 값은 true입니다.</param>
    void SetCurrentAnimation(std::string_view animKey, bool blend = true);
    void SetMainAnimation(std::string_view animKey, bool blend = true);

    /// <summary>애니메이션의 프레임을 설정합니다.</summary>
    /// <param name="frame">애니메이션 프레임 수</param>
    void SetCurrentAnimationFrame(float frame);
    void SetMainAnimationFrame(float frame);

    /// <summary>애니메이션의 플래그를 설정합니다.</summary>
    /// <param name="flags">애니메이션 플래그</param>
    void SetCurrentAnimationFlags(int flags);
    void SetMainAnimationFlags(int flags);

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

    inline size_t             GetOverrideAnimationCount()   const { return _overrideAnimationStack.size(); }
    inline const std::string& GetCurrentAnimationName()     const { return GetLastAnimationData().AnimationName; }
    inline float              GetCurrentAnimationTime()     const { return GetLastAnimationData().Duration; }
    inline float              GetCurrentAnimationSpeed()    const { return GetLastAnimationData().Speed; }
    inline bool               IsAnimationLooping()          const { return GetLastAnimationData().HasFlag(ANIMATION_FLAG_USE_LOOP); }
    inline bool               IsAnimationPlaying()          const { return !GetLastAnimationData().HasFlag(ANIMATION_FLAG_PAUSE); }

private:
    AnimationData                       _mainAnimationData;
    std::vector<AnimationData>          _overrideAnimationStack; 
    std::vector<std::function<void()>>  _eventQueue;
    bool                                _isBuildingOverrideAnimation = false;
};