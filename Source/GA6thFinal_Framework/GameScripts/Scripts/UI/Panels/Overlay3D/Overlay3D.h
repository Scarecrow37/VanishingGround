#pragma once
#include "GameScripts/Scripts/UI/Panels/Overlay/Overlaypanel.h"

class Overlay3DPanel : public OverlayPanel
{
    USING_PROPERTY(Overlay3DPanel)

public:
    Overlay3DPanel();

public:
    REFLECT_PROPERTY(TargetCameraTag, TargetObjectTag)

    GETTER(std::string, TargetCameraTag) { return ReflectFields->TargetCameraTag; }
    SETTER(std::string, TargetCameraTag) { ReflectFields->TargetCameraTag = value; }
    PROPERTY(TargetCameraTag)

	GETTER(std::string, TargetObjectTag) { return ReflectFields->TargetObjectTag; }
    SETTER(std::string, TargetObjectTag) { ReflectFields->TargetObjectTag = value; }
    PROPERTY(TargetObjectTag)



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
    std::string TargetObjectTag;
    REFLECT_FIELDS_END(Overlay3DPanel)

private:
    std::weak_ptr<CameraComponent> _targetCameraComponent;
    std::weak_ptr<GameObject>      _targetObject;

	POINT _offsetPoint;
};