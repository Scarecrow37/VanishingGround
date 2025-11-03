#pragma once
#include "GameScripts/Scripts/UI/Panels/Overlay/Overlaypanel.h"

class Overlay3DPanel : public OverlayPanel
{
    USING_PROPERTY(Overlay3DPanel)

public:
    Overlay3DPanel();

public:
    REFLECT_PROPERTY(TargetCameraTag)

    GETTER(std::string, TargetCameraTag) { return ReflectFields->TargetCameraTag; }
    SETTER(std::string, TargetCameraTag) { ReflectFields->TargetCameraTag = value; }
    PROPERTY(TargetCameraTag)

public:
    void SetPosition(const Vector3& position);

protected:
    void Update() override;

private:
    void UpdateCameraViewMatrix();

protected:
    REFLECT_FIELDS_BEGIN(OverlayPanel)
    std::string TargetCameraTag;
    std::string TargetObjectTag;
    REFLECT_FIELDS_END(Overlay3DPanel)

private:
	POINT _offsetFromTarget;
};