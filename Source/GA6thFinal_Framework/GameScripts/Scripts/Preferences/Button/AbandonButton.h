#pragma once
class AbandonButton : public Component, public InputReceiver
{
    USING_PROPERTY(AbandonButton)

public:
    AbandonButton();
    ~AbandonButton() override;

public:
    void Awake() override;
    void Update() override;
    void Reset() override;

public:
    REFLECT_PROPERTY()
private:
    void DirtyOnFlag(const Input::Controller&);

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(AbandonButton)
private:
    bool _dirtyFlag = false;
    GameObject* _abandonPannel;
};

