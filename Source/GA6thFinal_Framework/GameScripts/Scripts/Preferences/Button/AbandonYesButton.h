#pragma once
class AbandonYesButton : public Component ,public InputReceiver
{
    USING_PROPERTY(AbandonYesButton)

public:
    AbandonYesButton();
    ~AbandonYesButton() override;

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
    REFLECT_FIELDS_END(AbandonYesButton)
private:
    bool _dirtyFlag = false;
    class PreferencesManager* _preferencesManager;
};

