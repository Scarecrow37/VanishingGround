#pragma once
#include "UmFramework.h"

class Camera;
class CameraComponent : public Component
{
    USING_PROPERTY(CameraComponent)
public:
    REFLECT_PROPERTY(
        FOV, 
        Width, 
        Height,
        Aspect
        )

public:
    CameraComponent();
    virtual ~CameraComponent();

public:
    /*
    * 카메라의 설정 변경 여부입니다.
    */
    bool IsDirty() const { return _isDirty; }

    /*카메라의 투영 행렬을 업데이트 합니다.*/
    void UpdatePerspective();

    /*카메라의 뷰행렬을 업데이트 합니다.*/
    void UpdateView() const;

    /*이 카메라를 메인 카메라로 설정합니다.*/
    void SetMainCamera();

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

protected:
    REFLECT_FIELDS_BEGIN(Component)
    float FovDegree = 80.f;
    float Width = 1920.f;
    float Height = 1080.f;
    float Near = 0.1f;
    float Far = 1000.f;
    REFLECT_FIELDS_END(CameraComponent)

private:
    bool _isDirty;
    std::shared_ptr<Camera> _camera;
};
