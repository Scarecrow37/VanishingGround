#pragma once

class InputStackTestComponent : public Component, public InputReceiver
{
    USING_PROPERTY(InputStackTestComponent)
public:
    REFLECT_PROPERTY(ImageGuid)

    GETTER(const std::string&, ImageGuid) { return _image; }
    SETTER(const std::string&, ImageGuid) { _image = value; }
    PROPERTY(ImageGuid)
public:
    InputStackTestComponent();
    ~InputStackTestComponent() override;
protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(InputStackTestComponent)

    void Awake() override;
    void Start() override;

    void OnPush(const Input::Controller&);
    void OnPop(const Input::Controller&);

    std::string _image;
};
