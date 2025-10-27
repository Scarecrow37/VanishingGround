#pragma once

class InputStackTestComponent : public Component, public InputReceiver
{
    USING_PROPERTY(InputStackTestComponent)
public:
    REFLECT_PROPERTY(ImageGuid)

    GETTER(const std::string&, ImageGuid) { return ReflectFields->Image; }
    SETTER(const std::string&, ImageGuid) { ReflectFields->Image = value; }
    PROPERTY(ImageGuid)
public:
    InputStackTestComponent();
    ~InputStackTestComponent() override;
protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string Image;
    REFLECT_FIELDS_END(InputStackTestComponent)

    void Awake() override;
    void Start() override;

    void OnPush(const Input::Controller&);
    void OnPop(const Input::Controller&);
};
