#pragma once

class SceneGizmo;

class EGizmoManager
{
public:
    /*ImGuizmo 사용 상태를 추적합니다.*/
    enum class GuizmoState
    {
        IDLE,  //기본
        START, //사용시작
        USING, //사용중
        END    //사용종료
    };

    using SceneGizmoQueue = std::vector<std::pair<std::weak_ptr<Component>, SceneGizmo*>>; //씬 기즈모 Queue

    EGizmoManager();
    ~EGizmoManager() = default;

    /// <summary>
    /// 이번 프레임에 렌더링할 기즈모를 등록합니다.
    /// </summary>
    /// <param name="gizmo :">그릴 대상</param>
    void SubmitSceneGizmoIcon(SceneGizmo* gizmo);

    /// <summary>
    /// 이번 프레임에 렌더링할 ImGuizmo를 등록합니다.
    /// </summary>
    /// <param name="gizmo"></param>
    void SubminSceneImGuizmo(SceneGizmo* gizmo);

    /// <summary>
    /// 대상 Window와 Camera 기준으로 Gizmo를 그릴 준비를 합니다.
    /// </summary>
    /// <param name="targetWindow :">대상 ImGuiWindow</param>
    /// <param name="camera :">대상 Camera</param>
    void BeginDraw(ImGuiWindow* targetWindow, Camera* camera);

    /// <summary>
    /// 등록된 기즈모들을 Draw합니다. BeginDraw가 먼저 호출되야합니다.
    /// </summary>
    /// <param name="enableButton :">Scene Gizmo의 Button 활성화 여부를 결정합니다.</param>
    void Draw(bool enableButton = true);

    /// <summary>
    /// 카메라 기준으로 ImGuizmo를 Draw합니다.
    /// </summary>
    void DrawImGuizmo(ImGuiHelper::DrawManipulateDesc& desc);

    /// <summary>
    /// Draw를 종료합니다.
    /// </summary>
    void EndDraw();

    /// <summary>
    /// 씬 기즈모 아이콘 큐를 반환합니다.
    /// </summary>
    /// <returns>씬 기즈모 아이콘 큐</returns>
    const SceneGizmoQueue& GetSceneGizmoIconQueue() { return _sceneGizmosIcon; }

    /// <summary>
    /// 씬 ImGuizmo 큐를 반환합니다.
    /// </summary>
    /// <returns>ImGuizmo 큐</returns>
    const SceneGizmoQueue& GetSceneImGuizmoQueue() { return _sceneImGuizmos; }

    /// <summary>
    /// 현재 ImGuizmo 사용 상태를 반환합니다.
    /// </summary>
    /// <returns></returns>
    GuizmoState GetImGuizmoState() const { return _guizmoState; }

    /// <summary>
    /// ImGuizmo Over 여부를 반환합니다.
    /// </summary>
    /// <returns></returns>
    bool IsOverImGuizmo() const { return _isOver; }

private:
    SceneGizmoQueue _sceneGizmosIcon;
    SceneGizmoQueue _sceneImGuizmos;
    ImGuiWindow*    _targetWindow;
    Camera*         _targetCamera;
    GuizmoState     _guizmoState;
    bool            _isOver;

private:
    //출력할 위치를 계산합니다.
    bool CalculateGizmoScreenPosition(SceneGizmo& gizmo, ImVec2* outScreenPos);

};