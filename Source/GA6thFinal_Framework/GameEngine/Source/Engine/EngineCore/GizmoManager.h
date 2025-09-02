#pragma once

class SceneGizmo;

class EGizmoManager
{
public:
    using SceneGizmoQueue = std::vector<std::pair<std::weak_ptr<Component>, SceneGizmo*>>; //씬 기즈모 Queue

    EGizmoManager();
    ~EGizmoManager() = default;

    /// <summary>
    /// 이번 프레임에 렌더링할 기즈모를 등록합니다.
    /// </summary>
    /// <param name="gizmo :">그릴 대상</param>
    void SubmitSceneGizmo(SceneGizmo* gizmo);

    /// <summary>
    /// 대상 Window와 Camera 기준으로 Gizmo를 그릴 준비를 합니다.
    /// </summary>
    /// <param name="targetWindow :">대상 ImGuiWindow</param>
    /// <param name="camera :">대상 Camera</param>
    void BeginDraw(ImGuiWindow* targetWindow, Camera* camera);

    /// <summary>
    /// 등록된 기즈모들을 Draw합니다. BeginDraw가 먼저 호출되야합니다.
    /// </summary>
    void Draw();

    /// <summary>
    /// Draw를 종료합니다.
    /// </summary>
    void EndDraw();

    /// <summary>
    /// 씬 기즈모 큐를 반환합니다.
    /// </summary>
    /// <returns>씬 기즈모 큐</returns>
    const SceneGizmoQueue& GetSceneGizmoQueue() 
    { 
        return _sceneGizmos; 
    }

private:
    SceneGizmoQueue _sceneGizmos;
    ImGuiWindow*    _targetWindow;
    Camera*         _targetCamera;

private:
    //출력할 위치를 계산합니다.
    bool CalculateGizmoScreenPosition(SceneGizmo& gizmo, ImVec2* outScreenPos);

};