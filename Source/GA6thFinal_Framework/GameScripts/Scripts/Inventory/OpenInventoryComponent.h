#pragma once
class OpenInventoryComponent : public Component, public InputReceiver
{
    USING_PROPERTY(OpenInventoryComponent)

public:
    OpenInventoryComponent();
    ~OpenInventoryComponent() override;

public:
    GETTER(ControllerButton, OpenButton) { return ReflectFields->OpenButton; }
    SETTER(ControllerButton, OpenButton) { ReflectFields->OpenButton = value; }
    // type : InputReceiver::ControllerButton 
    // 인벤토리 열기 버튼으로 사용할 키
    PROPERTY(OpenButton)

    REFLECT_PROPERTY(OpenButton)
protected:
    REFLECT_FIELDS_BEGIN(Component)
    ControllerButton OpenButton = ControllerButton::UNKNOWN;
    REFLECT_FIELDS_END(OpenInventoryComponent)

    void Awake() override;
    void Update() override;
    void OnButtonOpen(const Input::Controller&);

    bool _openFlag;
};

