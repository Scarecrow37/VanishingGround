#pragma once
#include "GameScripts/Scripts/UI/Panels/Overlay/Overlaypanel.h"

class Overlay3DPanel : public OverlayPanel
{
    USING_PROPERTY(Overlay3DPanel)

public:
    Overlay3DPanel();

public:
    REFLECT_PROPERTY(TargetCameraTag, Z)

    GETTER(std::string, TargetCameraTag) { return ReflectFields->TargetCameraTag; }
    SETTER(std::string, TargetCameraTag) { ReflectFields->TargetCameraTag = value; }
    PROPERTY(TargetCameraTag)

    GETTER(float, Z) { return ReflectFields->Z; }
    SETTER(float, Z) { ReflectFields->Z = value; }
    PROPERTY(Z)

protected:
    void Awake() override;
    void Start() override;
    void Update() override;

private:
    void FindComponents();
    void UpdateCameraViewMatrix();

protected:
    REFLECT_FIELDS_BEGIN(OverlayPanel)
    std::string TargetCameraTag;
    float       Z = 0.0f;
    REFLECT_FIELDS_END(Overlay3DPanel)

private:
    std::weak_ptr<CameraComponent> _targetCameraComponent;

    POINT _originPoint;
    POINT _offsetAndMarginLT;
    Vector3 _targetWorldPosition;
};