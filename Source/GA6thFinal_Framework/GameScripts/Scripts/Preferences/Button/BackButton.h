#pragma once
class BackButton : public Component, public InputReceiver
{
    USING_PROPERTY(BackButton)

public:
    BackButton();
    ~BackButton() override;

public:
    void Awake() override;
    void Update() override;
    void Reset() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(BackButton)    
private:
    void DirtyOffFlag(const Input::Controller&);

private:
    bool _dirtyFlag = false;
    class PreferencesManager* _preferencesManager;
};

