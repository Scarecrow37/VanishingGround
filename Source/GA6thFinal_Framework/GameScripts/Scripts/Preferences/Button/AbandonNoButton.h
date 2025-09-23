#pragma once
class AbandonNoButton : public Component, public InputReceiver
{
    USING_PROPERTY(AbandonNoButton)

public:
    AbandonNoButton();
    ~AbandonNoButton() override;

public:
    void Awake() override;
    void Update() override;
    void Reset() override;

public:
    REFLECT_PROPERTY()
private:
    void DirtyOffFlag(const Input::Controller&);

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(AbandonNoButton)
private:
    bool        _dirtyFlag = false;
    GameObject* _abandonPannel;
};

