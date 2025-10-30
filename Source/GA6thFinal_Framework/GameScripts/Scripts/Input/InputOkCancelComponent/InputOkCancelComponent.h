#pragma once
class InputOkCancelComponent : public Component, public InputReceiver
{
    USING_PROPERTY(InputOkCancelComponent)

public:
    InputOkCancelComponent();
    ~InputOkCancelComponent() override;

public:
    void GetOkOrCancel(const std::function<void(bool)>& callback);

    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(InputOkCancelComponent)

    void Awake() override;
    void Update() override;

private:
    std::function<void(bool)> _callback;

    void OnOk(const Input::Controller&);
    void OnCancel(const Input::Controller&);

    bool _result = false;
    bool _onClose  = false;
};

