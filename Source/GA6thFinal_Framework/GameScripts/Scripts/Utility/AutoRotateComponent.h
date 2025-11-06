#pragma once
class AutoRotateComponent : public Component
{
    USING_PROPERTY(AutoRotateComponent)

public:
    AutoRotateComponent();
    ~AutoRotateComponent() override;

public:
    REFLECT_PROPERTY(
        RotateSpeedX,
        RotateSpeedY,
        RotateSpeedZ)

    GETTER(float, RotateSpeedX)
    {
        return ReflectFields->RotateSpeedX;
    }
    SETTER(float, RotateSpeedX)
    {
        ReflectFields->RotateSpeedX = value;
    }
    PROPERTY(RotateSpeedX)

    GETTER(float, RotateSpeedY)
    {
        return ReflectFields->RotateSpeedY;
    }
    SETTER(float, RotateSpeedY)
    {
        ReflectFields->RotateSpeedY = value;
    }
    PROPERTY(RotateSpeedY)

    GETTER(float, RotateSpeedZ)
    {
        return ReflectFields->RotateSpeedZ;
    }
    SETTER(float, RotateSpeedZ)
    {
        ReflectFields->RotateSpeedZ = value;
    }
    PROPERTY(RotateSpeedZ)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    float RotateSpeedX = 0.f; 
    float RotateSpeedY = 0.f; 
    float RotateSpeedZ = 0.f;  
    REFLECT_FIELDS_END(AutoRotateComponent)

    virtual void Reset() override;
    virtual void Update() override;

private:
    Vector3 _currentRotation{0.f, 0.f, 0.f};
};

