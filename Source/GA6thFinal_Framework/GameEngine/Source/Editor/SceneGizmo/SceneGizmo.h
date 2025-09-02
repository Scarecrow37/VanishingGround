#pragma once

/*Scene View에 클릭 가능한 아이콘을 생성합니다. 맴버 소유자는 컴포넌트여야 합니다.*/
class SceneGizmo
{
    friend class EGizmoManager;
public:
    enum class DefaultIcon
    {
        CAMERA,
        LIGHT,
    };

    inline static constexpr const wchar_t* GetIconPath(DefaultIcon icon)
    {
        switch (icon)
        {
        case SceneGizmo::DefaultIcon::CAMERA:
            return L"../GameEngine/Icon/Gizmo/camera.png";
        case SceneGizmo::DefaultIcon::LIGHT:
            return L"../GameEngine/Icon/Gizmo/light.png";
        default:
            return L"null";
        }
    }

    SceneGizmo(Component* owner);
    ~SceneGizmo() = default;

    /// <summary>
    /// 기즈모로 사용할 기본 텍스쳐를 로드합니다.
    /// </summary>
    /// <param name="icon :">사용할 아이콘ㄴ</param>
    void SetIconTexture(DefaultIcon icon);

    /// <summary>
    /// 기즈모로 사용할 텍스쳐를 로드합니다.
    /// </summary>
    /// <param name="path :">사용할 텍스쳐의 경로</param>
    void SetIconTexture(const File::Path& path);

    /// <summary>
    /// Scene View에 Gizmo를 Draw합니다. 한 프레임 마다 호출해야합니다.
    /// </summary>
    void Draw();

    /// <summary>
    /// 화면 기준 크기를 설정합니다.
    /// </summary>
    ImVec2 Size;

private:
    Component& _owner;
    std::shared_ptr<Texture> _icon;

};