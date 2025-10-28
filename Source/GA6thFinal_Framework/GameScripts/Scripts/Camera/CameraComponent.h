#pragma once
#include "UmFramework.h"

class CameraComponent : public Component
{
    USING_PROPERTY(CameraComponent)
protected:
    inline static const XMVECTORF32 DEBUG_COLOR = DirectX::Colors::Green;

public:
    /*현재 메인카메라로 설정된 카메라 컴포넌트를 반환합니다.*/
    static CameraComponent* MainCamera() { return ESceneManager::Engine::GetMainCamera(); }

public:
    REFLECT_PROPERTY(
        FOV, 
        Width, Height,
        Aspect, Near, Far,
        IsMainCamera
        )

public:
    CameraComponent();
    virtual ~CameraComponent();

public:
    /*
    * 카메라의 설정 변경 여부입니다.
    */
    bool IsDirty() const { return _isDirty; }

    /*Camera 객체를 설정합니다. 이미 설정되어 있으면 설정할 수 없습니다.*/
    bool SetTarget(const std::shared_ptr<Camera>& camera) 
    {
        bool result = false;
        if (nullptr != camera && nullptr == _camera)
        {
            result = true;
            _camera = camera;
            _isDirty = true;
            UpdatePerspective();
            UpdateView();
        }
        return result;
    }

    /*Camera를 반환합니다.*/
    const std::shared_ptr<Camera>& GetCamera() const { return _camera; }

    /*카메라의 투영 행렬을 업데이트 합니다.*/
    void UpdatePerspective()
    {
        if (nullptr != _camera && true == _isDirty)
        {
            float fov    = FOV;
            float aspect = Aspect;
            float nearZ  = Near;
            float farZ   = Far;
            _camera->SetupPerspective(fov, aspect, nearZ, farZ);
            _isDirty = false;
        }
    }

    /*카메라의 뷰행렬을 업데이트 합니다.*/
    void UpdateView() const
    {
        if (nullptr != _camera)
        {
            Transform& transform = gameObject->transform;
            const Matrix& worldMatrix = transform.GetWorldMatrix();
            _camera->SetWorldMatrix(worldMatrix);
        }
    }

    /*이 카메라를 메인 카메라로 설정합니다.*/
    void SetMainCamera()
    {
        if (nullptr != _camera)
        {
            UmGraphics.SetCamera("Game", _camera);
        }
        ReflectFields->IsMainCam = true;
        ESceneManager::Engine::SetSceneMainCamera(this);
    }

    void ResetMainCamera() 
    { 
        if (true == ReflectFields->IsMainCam)
        {
            ReflectFields->IsMainCam = false;
            ESceneManager::Engine::ResetSceneMainCamera();
        }
    }

    /// <summary>
    /// 월드 좌표를 NDC(-1 ~ 1)로 변환합니다.
    /// </summary>
    /// <param name="wolrdPos">변환할 월드 좌표를 나타내는 Vector3 객체입니다.</param>
    /// <returns>변환된 NDC 좌표를 나타내는 Vector3 객체를 반환합니다.</returns>
    Vector3 WorldToNDC(const Vector3& wolrdPos);
    static Vector3 WorldToNDC(const Vector3& wolrdPos, const Matrix& viewMatrix, const Matrix& projMatrix);

    /// <summary>
    /// 월드 좌표를 뷰포트 좌표로 변환합니다.
    /// </summary>
    /// <param name="worldPos">변환할 월드 좌표를 나타내는 Vector3 객체입니다.</param>
    /// <returns>변환된 뷰포트 좌표를 나타내는 Vector3 객체입니다.</returns>
    Vector3 WorldToViewport(const Vector3& worldPos);
    static Vector3 WorldToViewport(const Vector3& worldPos, const Matrix& viewMatrix, const Matrix& projMatrix);

    /// <summary>
    /// 뷰포트 좌표를 월드 좌표로 변환합니다.
    /// </summary>
    /// <param name="screenPos">변환할 뷰포트 좌표를 나타내는 Vector3 값입니다.</param>
    /// <returns>변환된 월드 좌표를 나타내는 Vector3 값입니다.</returns>
    Vector3 ViewportToWorld(const Vector3& screenPos);

public:
    GETTER(float, FOV) { return ReflectFields->FovDegree; }
    SETTER(float, FOV) 
    { 
        ReflectFields->FovDegree = std::max(value, 5.f); 
        _isDirty = true;
    }
    //카메라의 Field of View 입니다.
    PROPERTY(FOV)

    GETTER(float, Width) { return ReflectFields->Width; }
    SETTER(float, Width) 
    { 
        ReflectFields->Width = std::max(value, 100.f);
        _isDirty = true;
    }
    PROPERTY(Width)

    GETTER(float, Height) { return ReflectFields->Height; }
    SETTER(float, Height) 
    { 
        ReflectFields->Height = std::max(value, 100.f);
        _isDirty = true;
    }
    PROPERTY(Height)

    GETTER_ONLY(float, Aspect) { return ReflectFields->Width / ReflectFields->Height; }
    //카메라의 비율입니다.
    PROPERTY(Aspect)

    GETTER(float, Near) { return ReflectFields->Near; }
    SETTER(float, Near) 
    { 
        ReflectFields->Near = std::max(value, 0.1f);
        _isDirty = true;
    }
    PROPERTY(Near)

    GETTER(float, Far) { return ReflectFields->Far; }
    SETTER(float, Far) 
    { 
        ReflectFields->Far = std::max(value, 10.f);
        _isDirty = true;
    }
    PROPERTY(Far)

    GETTER_ONLY(bool, IsMainCamera)
    {
        return ReflectFields->IsMainCam;
    }
    //이 카메라의 메인 카메라 여부를 반환합니다.
    PROPERTY(IsMainCamera)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    float FovDegree = 80.f;
    float Width = 1920.f;
    float Height = 1080.f;
    float Near = 0.1f;
    float Far = 1000.f;
    bool IsMainCam = false;
    REFLECT_FIELDS_END(CameraComponent)

    virtual void Reset() override;

    /// <summary>
    /// <para>  ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다. </para>
    /// </summary>
    virtual void ImGuiDrawPropertysEvent() override;

    /// <summary>
    /// <para> 역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.  </para>
    /// <para> 직접 override 해서 사용합니다.                     </para>
    /// </summary>
    virtual void DeserializedReflectEvent() override;

    /// <summary>
    /// <para> 에디터 Scene View에 DrawDebug를 그리기 위한 함수입니다. </para>
    /// <para> 에디터 에서만 호출됩니다.                              </para>
    /// </summary>
    virtual void OnDrawDebug() override;

    /// <summary>
    /// <para> 에디터 Scene View에 DrawDebug를 그리기 위한 함수입니다. </para>
    /// <para> 컴포넌트가 Inspector에 선택되었을때만 호출됩니다.       </para>
    /// <para> 에디터 에서만 호출됩니다. </para>
    /// </summary>
    virtual void OnDrawDebugSelected() override;

private:
    bool _isDirty;

protected:
    std::shared_ptr<Camera> _camera;

private:
#ifdef _UMEDITOR
    SceneGizmo _gizmo{this};
#endif
};
