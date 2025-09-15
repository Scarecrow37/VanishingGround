#pragma once

/*Scene View에 클릭 가능한 아이콘을 생성합니다. 오너 컴포넌트가 존재해야합니다.*/
class SceneGizmo
{
    friend class EGizmoManager;
public:
    enum class DefaultIcon
    {
        LIGHT,
        CAMERA,
    };

    inline static constexpr const wchar_t* GetIconPath(DefaultIcon icon)
    {
        switch (icon)
        {
        case SceneGizmo::DefaultIcon::LIGHT:
            return L"../GameEngine/Icon/Gizmo/light.png";
        case SceneGizmo::DefaultIcon::CAMERA:
            return L"../GameEngine/Icon/Gizmo/camera.png";
        default:
            return L"null";
        }
    }

    SceneGizmo(Component* ownerComponenet);
    ~SceneGizmo() = default;

    /// <summary>
    /// 기즈모로 사용할 기본 텍스쳐를 로드합니다.
    /// </summary>
    /// <param name="icon :">사용할 아이콘</param>
    void SetIconTexture(DefaultIcon icon);

    /// <summary>
    /// 기즈모로 사용할 텍스쳐를 로드합니다.
    /// </summary>
    /// <param name="path :">사용할 텍스쳐의 경로</param>
    void SetIconTexture(const File::Path& path);

    /// <summary>
    /// Scene View에 Gizmo Icon을 Draw합니다. 한 프레임 마다 호출해야합니다.
    /// </summary>
    void DrawIcon();

    /// <summary>
    /// Scene View에 Gizmo Manipulate를 Draw합니다. _ownerMatrix가 존재해야 합니다.
    /// </summary>
    void DrawImGuizmo();

    /// <summary>
    /// 화면 기준 크기를 설정합니다.
    /// </summary>
    ImVec2 Size;

    /// <summary>
    /// 기즈모가 클릭될때 호출될 이벤트함수를 등록합니다. 등록된 함수가 없으면 오너 컴포넌트를 인스펙터에 선택합니다.
    /// </summary>
    UmDelegate<> EventListener;

    /// <summary>
    /// 기즈모가 그려질 매트릭스를 설정합니다.
    /// 기본적으로 Component의 행렬을 사용합니다.
    /// </summary>
    /// <param name="ownerMatrix :">설정할 매트릭스</param>
    void SetOwnerMatrix(Matrix& ownerMatrix) { _ownerMatrix = &ownerMatrix; }

    /// <summary>
    /// 기즈모가 그려질 행렬을 Owner Component 기준으로 설정합니다.
    /// </summary>
    /// <param name="ownerMatrix"></param>
    void ResetOwnerMatrix() 
    { 
        _drawManipulate = false;
        _ownerMatrix = nullptr; 
    }

private:
    Component&               _ownerComponenet;
    Matrix*                  _ownerMatrix    = nullptr;
    bool                     _drawManipulate = false;
    std::shared_ptr<Texture> _icon;

};