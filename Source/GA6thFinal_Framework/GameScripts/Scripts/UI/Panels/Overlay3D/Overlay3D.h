#pragma once
#include "GameScripts/Scripts/UI/Panels/Overlay/Overlaypanel.h"

class Overlay3DPanel : public OverlayPanel
{
    USING_PROPERTY(Overlay3DPanel)

public:
    Overlay3DPanel();

public:
    REFLECT_PROPERTY(TargetCameraTag, UpDistance)

    GETTER(std::string, TargetCameraTag) { return ReflectFields->TargetCameraTag; }
    SETTER(std::string, TargetCameraTag) { ReflectFields->TargetCameraTag = value; }
    PROPERTY(TargetCameraTag)

    GETTER(int, UpDistance) { return static_cast<int>(ReflectFields->UpDistance); }
    SETTER(int, UpDistance) { ReflectFields->UpDistance = static_cast<LONG>(value); }
    PROPERTY(UpDistance)

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
    LONG        UpDistance;
    REFLECT_FIELDS_END(Overlay3DPanel)

private:
	POINT _offsetFromTarget;
};