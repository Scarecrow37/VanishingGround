#pragma once
#include "Animation/Structs/AnimationData.h"

class SkeletalMeshRenderer;
class MeshRenderer;
class Model;
class Animator;

namespace Timeline
{
    class EventContext;
    class EventTrack;
} // namespace Timeline

class AnimationComponent : public Component
{
    USING_PROPERTY(AnimationComponent)
    using EventQueue = std::vector<std::function<void()>>;

public:
    void Reset() override;
    void Awake() override;
    void OnDestroy() override;  
    void Update() override;
    void OnDrawDebug() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

private:
    AnimationData& GetLastAnimationDataEx();

    void UpdateAnimation(AnimationData& animData);
    void SetAnimationEx(AnimationData& animData);
    void ChangeAnimationEx(AnimationData& animData, std::string_view animKey, bool blend);
    void ChangeAnimationFrameEx(AnimationData& animData, float frame);
    void ChangeAnimationFlagsEx(AnimationData& animData, int flags);
    void SetAnimationPopCallbackEx(AnimationData& animData, std::function<void()> callback);
    void GetAnimationNameEx(std::string_view key, std::string& str) const;

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
    bool PushOverrideAnimation(std::string_view animKey, bool blend = true, std::function<bool(const AnimationData&)> popCondition = nullptr);

    /// <summary>애니메이션 오버라이드를 뺍니다.</summary>
    void PopOverrideAnimation();

    /// <summary>현재 애니메이션을 바꿉니다.</summary>
    /// <param name="animKey">애니메이션 키</param>
    /// <param name="loop">루프 여부. 기본 값은 true입니다.</param>
    void ChangeCurrentAnimation(std::string_view animKey, bool blend = true);
    void ChangeMainAnimation(std::string_view animKey, bool blend = true);

    /// <summary>애니메이션의 프레임을 설정합니다.</summary>
    /// <param name="frame">애니메이션 프레임 수</param>
    void ChangeCurrentAnimationFrame(float frame);
    void ChangeMainAnimationFrame(float frame);

    /// <summary>애니메이션의 플래그를 설정합니다.</summary>
    /// <param name="flags">애니메이션 플래그</param>
    void ChangeCurrentAnimationFlags(int flags);
    void ChangeMainAnimationFlags(int flags);

    void SetCurrentAnimationPopCallback(std::function<void()> callback);
    void SetMainAnimationPopCallback(std::function<void()> callback);

    const AnimationData& GetMainAnimationData() const;
    const AnimationData& GetLastAnimationData() const;

    inline size_t GetOverrideAnimationCount()   const { return _overrideAnimationStack.size(); }
    inline bool   IsAnimationLooping()          const { return GetLastAnimationData().HasFlag(ANIMATION_FLAG_USE_LOOP); }
    inline bool   IsAnimationPaused()           const { return GetLastAnimationData().HasFlag(ANIMATION_FLAG_PAUSE); }

    /// <summary>애니메이션을 0프레임으로 돌아간 후 재생합니다.</summary>
    void PlayCurrentAnimation();
    /// <summary>애니메이션을 재생합니다.</summary>
    void ResumeCurrentAnimation();
    /// <summary>애니메이션을 0프레임으로 돌아간 후 정지합니다.</summary>
    void StopCurrentAnimation();
    /// <summary>애니메이션을 일시 정지합니다.</summary>
    void PauseCurrentAnimation();

    void SetAnimator(SkeletalMeshRenderer* renderer);
    void SetAnimator(std::shared_ptr<Animator> animator);
    void UpdateNullAnimator();

    void SetAnimationEventTrackFromPath(const File::Path& path);
    void SetAnimationEventTrackFromGuid(const File::Guid& guid);
    void SetAnimationPreEventCallback(std::function<bool(const Timeline::EventContext*)> callback) { _preEventCallback = callback; }
    void SetAnimationPostEventCallback(std::function<void(const Timeline::EventContext*)> callback) { _postEventCallback = callback; }
    inline AnimationEventTrack& GetEventTrack() { return _eventTrack; }

    void AddAnimationMappingKey(std::string_view key, std::string_view animKey);
    void RemoveAnimationMappingKey(std::string_view key);
    bool HasAnimationMappingKey(std::string_view key) const;
    const std::string& GetAnimationNameFromKey(std::string_view key) const;

private:
    std::shared_ptr<Animator>  _animator;
    EventQueue                 _eventQueue;
    AnimationData              _mainAnimationData;
    std::vector<AnimationData> _overrideAnimationStack; 
    bool                       _isBuildingOverrideAnimation = false;
    REFLECT_FIELDS_BEGIN(Component)
    float       AnimationSpeedScale = 1.0f;
    std::string MainAnimationKey    = "";
    int         MainAnimationFlags  = ANIMATION_FLAG_NONE;
    bool        MainAnimationSpeed  = true;
    std::string AnimEventTrackGuid  = "";
    std::map<std::string, std::string> AnimationKeyMap;
    REFLECT_FIELDS_END(AnimationComponent)

    ///////////////////////////////////////////////////////////////////////
    /// EventTrack
    ///////////////////////////////////////////////////////////////////////

    File::GuidRef       _guidRef;
    File::Path          _filePath;
    AnimationEventTrack _eventTrack;
    std::function<bool(const Timeline::EventContext*)> _preEventCallback;  // Event Callback Function
    std::function<void(const Timeline::EventContext*)> _postEventCallback; // Event Callback Function
    std::string _selectedEventTrack;
    std::vector<std::function<void()>> _delayProcess;

};
