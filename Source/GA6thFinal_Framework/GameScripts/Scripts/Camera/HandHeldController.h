#pragma once
class HandHeldController : public Component
{
    USING_PROPERTY(HandHeldController)
public:
    REFLECT_PROPERTY(Intensity, Frequency, Interval)
    GETTER(float, Intensity) { return ReflectFields->ShakeIntensity; }
    SETTER(float, Intensity) 
    { 
        ReflectFields->ShakeIntensity = value; 
        if (_mainCam)
        {
            _mainCam->SetHandHeldIntensity(value);
        }
        isDirty                       = true;
    }
    PROPERTY(Intensity)
    GETTER(float, Frequency) { return ReflectFields->ShakeFrequency; }
    SETTER(float, Frequency) 
    { 
        ReflectFields->ShakeFrequency = value;
        if (_mainCam)
        {
            _mainCam->SetHandHeldFrequency(value);
        }
        isDirty                       = true;
    }
    PROPERTY(Frequency)
    GETTER(float, Interval) { return ReflectFields->ShakeInterval; }
    SETTER(float, Interval) 
    { 
        ReflectFields->ShakeInterval = value; 
        if (_mainCam)
        {
            _mainCam->SetHandHeldInterval(value);
        }
        isDirty                      = true;
    }
    PROPERTY(Interval)

public:
    HandHeldController();
    ~HandHeldController() override;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    float ShakeIntensity = 0.03f;
    float ShakeFrequency = 0.25f;
    float ShakeInterval  = 0.1f;
    REFLECT_FIELDS_END(HandHeldController)

    void Start() override;
    void Update() override;
    void OnDrawDebug() override;
    void OnDrawDebugSelected() override;

    class UmCineMotion* _mainCam = nullptr;
    bool                isDirty  = false;
};

