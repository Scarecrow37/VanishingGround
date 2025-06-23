#include "CameraComponent.h"
CameraComponent::CameraComponent() 
    : 
    Component(TYPE::CAMERA) 
{

}
CameraComponent::~CameraComponent() = default;

void CameraComponent::UpdatePerspective()
{
    if (nullptr != _camera && true == _isDirty)
    {
        float fov = FOV;
        float aspect = Aspect;
        float nearZ = Near;
        float farZ = Far;
        _camera->SetupPerspective(fov, aspect, nearZ, farZ);
        _isDirty = false;
    }
}

void CameraComponent::UpdateView() const 
{
    if (nullptr != _camera)
    {
        _camera->Update();
    }
}

void CameraComponent::SetMainCamera() 
{
    if (nullptr != _camera)
    {

    }
}
