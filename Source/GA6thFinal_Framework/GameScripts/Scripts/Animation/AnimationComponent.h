#pragma once
#include "Animation/Structs/AnimationData.h"

class SkeletalMeshRenderer;
class IMeshRenderer;
class Model;
class IAnimator;

namespace Timeline
{
    class EventContext;
    class EventTrack;
} // namespace Timeline

/// <summary>
/// AnimationComponent 클래스는 게임 오브젝트의 애니메이션 상태를 관리하고, 
/// 애니메이션의 재생, 정지, 일시정지, 프레임 및 플래그 변경, 
/// 오버라이드 애니메이션 스택 관리, 애니메이션 이벤트 트랙 및 매핑 기능을 제공합니다.
/// 
/// <para>오버라이드 애니메이션: 메인 애니메이션에 덧씌울 수 있는 애니메이션입니다.</para>
/// <para>애니메이션 이벤트 트랙: 애니메이션의 특정 프레임에 이벤트를 연결할 수 있는 기능입니다.</para>
/// </summary>
class AnimationComponent : public Component
{
    USING_PROPERTY(AnimationComponent)
    using EventQueue = std::vector<std::function<void()>>;

public:
    void Reset() override;
    void Start() override;
    void Update() override;
    void OnDestroy() override;
    void OnEnable() override;
    void OnDisable() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

private:
    AnimationData& GetLastAnimationDataEx();
    AnimationData& GetFrontAnimationDataEx();
    AnimationData& GetTopAnimationDataEx();

    void UpdateValidation(); // 유효성 검사 및 처리 업데이트
    void UpdateAnimation(AnimationData& animData);
    bool SetAnimationEx(AnimationData& animData);
    bool ChangeAnimationEx(AnimationData& animData, std::string_view animKey);
    void ChangeAnimationFrameEx(AnimationData& animData, float frame);
    void ChangeAnimationFlagsEx(AnimationData& animData, int flags);
    void SetAnimationPopCallbackEx(AnimationData& animData, std::function<void()> callback);
    void SetAnimationEnterCallbackEx(AnimationData& animData, std::function<void()> callback);
    void SetAnimationExitCallbackEx(AnimationData& animData, std::function<void()> callback);
    void SetAnimationEndCallbackEx(AnimationData& animData, std::function<void()> callback);
    void GetAnimationNameEx(std::string_view key, std::string& str) const;

public:
    void SetAnimator(SkeletalMeshRenderer* renderer);
    void SetAnimator(GraphicsPointer<IAnimator> animator);

    /// <summary>
    /// 다음에 적용할 애니메이션 플래그를 설정합니다.
    /// <para>해당 함수를 호출하지 않아도 애니메이션은 추가할 수 있지만, 기본 플래그로 적용됩니다.</para>
    /// </summary>
    /// <param name="nextAnimFlag">설정할 다음 애니메이션 옵션입니다.</param>
    void SetNextAnimationFlags(AnimationFlags nextAnimFlag);

    /// <summary>
    /// 애니메이션 오버라이드를 모두 지웁니다. 
    /// </summary>
    void ClearOverrideAnimations();

    /// <summary>
    /// <para>오버라이드 애니메이션 빌드를 시작합니다.</para>
    /// <para>빌드 중에는 애니메이션이 바뀌지 않고, End를 호출할 때 마지막 애니메이션으로 바꿉니다.</para>
    /// <para>Animation Blend가 의도적으로 되지 않는 현상을 방지하기 위해 사용합니다.</para>
    /// </summary>
    void BeginBuildOverrideAnimation();

    /// <summary>
    /// <para>오버라이드 애니메이션 빌드를 끝냅니다.</para>
    /// <para>빌드 종료 시점에서의 마지막 애니메이션으로 바꿉니다.</para>
    /// </summary>
    void EndBuildOverrideAnimation();

    /// <summary>
    /// <para>오버라이드 애니메이션을 추가합니다.</para>
    /// <para>Pop조건을 추가할 수 있습니다. (댕글링 위험성이 있는 콜백을 넣지 마세요.)</para>
    /// </summary>
    /// <param name="animKey">애니메이션 키</param>
    /// <param name="allowOverlap">마지막 애니메이션이 동일해도 중복 추가할 수 있는지 여부</param>
    /// <param name="popCondition">애니메이션 Pop 조건</param>
    /// <returns>보통 애니메이션 키가 유효하면 true를 반환합니다. 유효하지 않는 경우 false를 반환합니다.</returns>
    bool PushBackOverrideAnimation(std::string_view animKey, bool allowOverlap = false);
    bool PushFrontOverrideAnimation(std::string_view animKey, bool allowOverlap = false);

    /// <summary>
    /// <para>최상단 오버라이드 애니메이션을 제거하고, 마지막 애니메이션으로 바꿉니다.</para>
    /// <para>오버라이드 애니메이션이 비었다면, 메인 에니메이션을 적용합니다.</para>
    /// </summary>
    void PopOverrideAnimation();

    /// <summary>현재 애니메이션을 바꿉니다.</summary>
    /// <param name="animKey">애니메이션 키</param>
    /// <param name="loop">루프 여부. 기본 값은 true입니다.</param>
    bool ChangeCurrentAnimation(std::string_view animKey, bool resetFrame = true);
    bool ChangeMainAnimation(std::string_view animKey, bool resetFrame = true);

    /// <summary>애니메이션의 프레임을 설정합니다.</summary>
    /// <param name="frame">애니메이션 프레임 수</param>
    void ChangeCurrentAnimationFrame(float frame);
    void ChangeMainAnimationFrame(float frame);

    /// <summary>애니메이션의 플래그를 설정합니다.</summary>
    /// <param name="flags">애니메이션 플래그</param>
    void ChangeCurrentAnimationFlags(int flags);
    void ChangeMainAnimationFlags(int flags);

    /// <summary>애니메이션이 Pop될 때 호출되는 콜백을 설정합니다. (댕글링 위험성이 있는 콜백을 넣지 마세요.)</summary>
    void SetCurrentAnimationPopCallback(std::function<void()> callback);
    void SetMainAnimationPopCallback(std::function<void()> callback);

    /// <summary>애니메이션이 끝날 때 호출되는 콜백을 설정합니다. (댕글링 위험성이 있는 콜백을 넣지 마세요.)(루프는 끝날 때 마다 호출됩니다.)</summary>
    void SetCurrentAnimationEndCallback(std::function<void()> callback);
    void SetMainAnimationEndCallback(std::function<void()> callback);

    /// <summary>애니메이션을 진입할 때 호출되는 콜백을 설정합니다. (댕글링 위험성이 있는 콜백을 넣지 마세요.)(루프는 끝날 때 마다 호출됩니다.)</summary>
    void SetCurrentAnimationEnterCallback(std::function<void()> callback);
    void SetMainAnimationEnterCallback(std::function<void()> callback);

    /// <summary>애니메이션을 진입할 때 호출되는 콜백을 설정합니다. (댕글링 위험성이 있는 콜백을 넣지 마세요.)(루프는 끝날 때 마다 호출됩니다.)</summary>
    void SetCurrentAnimationExitCallback(std::function<void()> callback);
    void SetMainAnimationExitCallback(std::function<void()> callback);

    /// <summary>오버라이드 애니메이션이 Pop될 조건을 설정합니다. 설정하지 않는 경우 직접 Pop하기 전까지 유지됩니다.</summary>
    void SetCurrentAnimationPopCondition(std::function<bool(const AnimationData&)> callback);
    
    /// <summary>메인 애니메이션 데이터에 접근합니다.</summary>
    const AnimationData& GetMainAnimationData() const;
    /// <summary>최상단 애니메이션 데이터에 접근합니다.</summary>
    const AnimationData& GetFrontAnimationData() const;
    /// <summary>최상단 애니메이션 데이터에 접근합니다.</summary>
    const AnimationData& GetTopAnimationData() const;
    /// <summary>마지막(최상단) 애니메이션 데이터에 접근합니다.</summary>
    const AnimationData& GetLastAnimationData() const;

    /// <summary>현재 오버라이드 애니메이션 카운트를 계산합니다.</summary>
    inline size_t GetOverrideAnimationCount()   const { return _overrideAnimationStack.size(); }
    /// <summary>현재 애니메이션이 루프 상태인지 확인합니다.</summary>
    inline bool   IsAnimationLooping()          const { return GetTopAnimationData().HasFlag(ANIMATION_FLAG_USE_LOOP); }
    /// <summary>현재 애니메이션이 정지 중인지 확인합니다.</summary>
    inline bool   IsAnimationPaused()           const { return GetTopAnimationData().HasFlag(ANIMATION_FLAG_PAUSE); }

    /// <summary>애니메이션을 0프레임으로 돌아간 후 재생합니다.</summary>
    void PlayCurrentAnimation();
    /// <summary>애니메이션을 재생합니다.</summary>
    void ResumeCurrentAnimation();
    /// <summary>애니메이션을 0프레임으로 돌아간 후 정지합니다.</summary>
    void StopCurrentAnimation();
    /// <summary>애니메이션을 일시 정지합니다.</summary>
    void PauseCurrentAnimation();

    // ==Animation Track== //

    /// <summary>
    /// 지정된 경로의 애니메이션 이벤트 트랙을 설정합니다.
    /// </summary>
    /// <param name="path">애니메이션 이벤트 트랙을 설정할 파일의 경로입니다.</param>
    void SetAnimationEventTrackFromPath(const File::Path& path);
    /// <summary>
    /// 지정된 GUID를 사용하여 애니메이션 이벤트 트랙을 설정합니다.
    /// </summary>
    /// <param name="guid">애니메이션 이벤트 트랙을 식별하는 File::Guid 객체입니다.</param>
    void SetAnimationEventTrackFromGuid(const File::Guid& guid);
    /// <summary>
    /// 타임라인 이벤트가 호출되기 전에 호출될 콜백 함수를 설정합니다.
    /// </summary>
    /// <param name="callback">Timeline::EventContext 포인터를 인자로 받아 bool 값을 반환하는 콜백 함수입니다. 이벤트 발생 전 호출됩니다.</param>
    inline void SetAnimationPreEventCallback(std::function<bool(const Timeline::EventContext*)> callback) { _preEventCallback = callback; }
    /// <summary>
    /// 타임라인 이벤트가 호출된 이후에 호출될 콜백 함수를 설정합니다.
    /// </summary>
    /// <param name="callback">Timeline::EventContext 포인터를 인자로 받는 콜백 함수 객체입니다. 이벤트 발생 후 호출됩니다.</param>
    inline void SetAnimationPostEventCallback(std::function<void(const Timeline::EventContext*)> callback) { _postEventCallback = callback; }
    /// <summary>
    /// 이 함수는 내부의 AnimationEventTrack 객체에 대한 참조를 반환합니다.
    /// </summary>
    /// <returns>내부에 저장된 AnimationEventTrack 객체에 대한 참조를 반환합니다.</returns>
    inline AnimationEventTrack& GetAnimationEventTrack() { return _eventTrack; }
    inline std::shared_ptr<Timeline::EventTrack> GetCurrentEventTrack() { return _eventTrack.GetEventTrack(GetTopAnimationDataEx().GetAnimationName()); }
    /// <summary>
    /// 애니메이션 알림 기능을 활성화 또는 비활성화합니다.
    /// </summary>
    /// <param name="disable">애니메이션 알림을 비활성화할지 여부를 지정하는 불리언 값입니다.</param>
    inline void SetDisableAnimationNotify(bool disable) { ReflectFields->DisableAnimationNotify = disable; }

    // ==Animation Mapping==  //
    
    /// <summary>
    /// 애니메이션 매핑 키를 추가합니다.
    /// </summary>
    /// <param name="key">매핑에 사용할 키입니다.</param>
    /// <param name="animKey">연결할 애니메이션 원본 키입니다.</param>
    void AddAnimationMappingKey(std::string_view key, std::string_view animKey);
    /// <summary>
    /// 지정된 키에 해당하는 애니메이션 매핑을 제거합니다.
    /// </summary>
    /// <param name="key">제거할 애니메이션 매핑의 키입니다.</param>
    void RemoveAnimationMappingKey(std::string_view key);
    /// <summary>
    /// 주어진 키에 해당하는 애니메이션 매핑이 존재하는지 확인합니다.
    /// </summary>
    /// <param name="key">확인할 애니메이션 매핑 키입니다.</param>
    /// <returns>키에 해당하는 애니메이션 매핑이 있으면 true, 없으면 false를 반환합니다.</returns>
    bool HasAnimationMappingKey(std::string_view key) const;
    /// <summary>
    /// 주어진 키에 해당하는 애니메이션 이름을 반환합니다.
    /// </summary>
    /// <param name="key">애니메이션 이름을 조회할 키입니다.</param>
    /// <returns>키에 해당하는 애니메이션 이름에 대한 상수 참조를 반환합니다.</returns>
    const std::string& GetAnimationNameFromKey(std::string_view key) const;

    inline const std::map<std::string, std::string>& GetAnimationKeyMap() const { return ReflectFields->AnimationKeyMap; }

private:
    GraphicsPointer<IAnimator> _animator;
    EventQueue                 _eventQueue;
    AnimationData*             _currentAnimationData = nullptr; // 현재 애니메이션 데이터
    AnimationData              _mainAnimationData;
    std::deque<AnimationData>  _overrideAnimationStack; 
    AnimationData*             _lastAnimationData = nullptr;
    bool                       _isBuildingOverrideAnimation = false;
    UINT                       _prevBeginBuildAnimatonID    = 0;
    
    std::pair<bool, AnimationFlags> _nextAnimationFlag; // 다음 애니메이션 데이터 (first: isValid, second: NextAnimationData)
    
    // 지연 처리용 큐
    std::vector<std::function<void()>> _delayProcess;

    REFLECT_FIELDS_BEGIN(Component)
    float       AnimationSpeedScale = 1.0f;
    std::string MainAnimationKey    = "";
    int         MainAnimationFlags  = ANIMATION_FLAG_NONE;
    bool        MainAnimationSpeed  = true;
    std::string AnimEventTrackGuid  = "";
    bool        DisableAnimationNotify = false;
    std::map<std::string, std::string> AnimationKeyMap;
    REFLECT_FIELDS_END(AnimationComponent)

    ///////////////////////////////////////////////////////////////////////
    /// EventTrack
    ///////////////////////////////////////////////////////////////////////

    File::Guid          _trackGuid;
    File::Path          _trackPath;
    AnimationEventTrack _eventTrack;
    std::function<bool(const Timeline::EventContext*)> _preEventCallback;  // Event Callback Function
    std::function<void(const Timeline::EventContext*)> _postEventCallback; // Event Callback Function
    std::string _selectedEventTrack;    
};
