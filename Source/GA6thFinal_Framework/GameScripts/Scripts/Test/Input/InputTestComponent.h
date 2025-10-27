#pragma once

class InputTestComponent : public Component, public InputReceiver
{
    USING_PROPERTY(InputTestComponent)
public:
    REFLECT_PROPERTY(MoveSpeed, RotSpeed, Velocity)

public:
    InputTestComponent();
    virtual ~InputTestComponent();

    GETTER(float, MoveSpeed) { return ReflectFields->MoveSpeed; }
    SETTER(float, MoveSpeed) { ReflectFields->MoveSpeed = value; }
    PROPERTY(MoveSpeed)

    GETTER(float, RotSpeed) { return ReflectFields->RotSpeed; }
    SETTER(float, RotSpeed) { ReflectFields->RotSpeed = value; }
    PROPERTY(RotSpeed)

    GETTER_ONLY(const Vector3, Velocity) { return _velocity; }
    PROPERTY(Velocity)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    float MoveSpeed = 5.f;
    float RotSpeed  = 360.f;
    REFLECT_FIELDS_END(InputTestComponent)

    virtual void Awake() override;
    virtual void Update() override;
private:
    void  UpdateInput();

    float _leftTrigger;
    float _rightTrigger;
    Vector3 _velocity;

    Input::Controller::ThumbStickAxis _leftStickAxis;
    Input::Controller::ThumbStickAxis _rightStickAxis;

    void OnButton(const Input::Controller& controller);
    void OnTrigger(const Input::Controller& controller);
    void OnThumbStick(const Input::Controller& controller);
};
