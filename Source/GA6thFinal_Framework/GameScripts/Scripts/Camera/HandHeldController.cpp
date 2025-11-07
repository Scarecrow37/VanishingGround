#include "pchScripts.h"
#include "Camera/UmCineMotion.h"
#include "HandHeldController.h"

UMREAL_COMPONENT(HandHeldController)

HandHeldController::HandHeldController() = default;
HandHeldController::~HandHeldController() = default;

void HandHeldController::Start()
{
    if (_mainCam = GetComponent<UmCineMotion>())
    {
        _mainCam->SetHandHeldIntensity(ReflectFields->ShakeIntensity);
        _mainCam->SetHandHeldFrequency(ReflectFields->ShakeFrequency);
        _mainCam->BeginHandHeldShake();
    }
}

void HandHeldController::Update() 
{
    if (isDirty && _mainCam)
    {
        _mainCam->SetHandHeldIntensity(ReflectFields->ShakeIntensity);
        _mainCam->SetHandHeldFrequency(ReflectFields->ShakeFrequency);
        _mainCam->BeginHandHeldShake();
        isDirty = false;
    }
}

void HandHeldController::OnDrawDebug() 
{
    if (isDirty && _mainCam)
    {
        _mainCam->SetHandHeldIntensity(ReflectFields->ShakeIntensity);
        _mainCam->SetHandHeldFrequency(ReflectFields->ShakeFrequency);
        _mainCam->BeginHandHeldShake();
        isDirty = false;
    }
}

void HandHeldController::OnDrawDebugSelected() 
{
    if (isDirty && _mainCam)
    {
        _mainCam->SetHandHeldIntensity(ReflectFields->ShakeIntensity);
        _mainCam->SetHandHeldFrequency(ReflectFields->ShakeFrequency);
        _mainCam->BeginHandHeldShake();
        isDirty = false;
    }
}
