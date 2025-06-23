#pragma once
#include "UmFramework.h"

class CameraComponent : public Component
{
    USING_PROPERTY(CameraComponent)
public:
    REFLECT_PROPERTY(
        FOV, 
        Width, Height,
        Aspect, IsMainCamera
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
            if (true == ReflectFields->IsMainCam)
            {
                UmRenderer.SetCamera("Game", _camera);
            }
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
            UmRenderer.SetCamera("Game", _camera);
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

public:
    GETTER(float, FOV) { return ReflectFields->FovDegree; }
    SETTER(float, FOV) 
    { 
        ReflectFields->FovDegree = value; 
        _isDirty = true;
    }
    //카메라의 Field of View 입니다.
    PROPERTY(FOV)

    GETTER(float, Width) { return ReflectFields->Width; }
    SETTER(float, Width) 
    { 
        ReflectFields->Width = value; 
        _isDirty = true;
    }
    PROPERTY(Width)

    GETTER(float, Height) { return ReflectFields->Height; }
    SETTER(float, Height) 
    { 
        ReflectFields->Height = value; 
        _isDirty = true;
    }
    PROPERTY(Height)

    GETTER_ONLY(float, Aspect) { return ReflectFields->Width / ReflectFields->Height; }
    //카메라의 비율입니다.
    PROPERTY(Aspect)

    GETTER(float, Near) { return ReflectFields->Near; }
    SETTER(float, Near) 
    { 
        ReflectFields->Near = value; 
        _isDirty = true;
    }
    PROPERTY(Near)

    GETTER(float, Far) { return ReflectFields->Far; }
    SETTER(float, Far) 
    { 
        ReflectFields->Far = value; 
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

    /// <summary>
    /// <para>  ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다. </para>
    /// </summary>
    virtual void ImGuiDrawPropertysEvent() override;

    /// <summary>
    /// <para> 역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.  </para>
    /// <para> 직접 override 해서 사용합니다.                     </para>
    /// </summary>
    virtual void DeserializedReflectEvent() override;

private:
    bool _isDirty;
    std::shared_ptr<Camera> _camera;
};
