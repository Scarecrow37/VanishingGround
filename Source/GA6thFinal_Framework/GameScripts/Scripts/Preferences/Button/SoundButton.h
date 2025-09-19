#pragma once
constexpr int MaxVolume = 10;
class SoundButton : public UINavigationComponent, public InputReceiver
{
    USING_PROPERTY(SoundButton)

public:
    SoundButton();
    ~SoundButton() override;

public:
    void Awake() override;
    void Start() override;
    void Reset() override;
    void Update() override;

public:
    REFLECT_PROPERTY()
protected:
    void FocusIn() override;
    void FocusOut() override;

private:
    void ControlVolumeUp(const Input::Controller& controller);
    void ControlVolumeDown(const Input::Controller& controller);
    void ControlVolumeStick(const Input::Controller& controller);
    void GetChildObject();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(SoundButton)
private:
    GameObject*                 _leftArrow;
    GameObject*                 _rightArrow;
    GameObject*                 _title[2];
    GameObject*                 _volumeBar;
    GameObject*                 _volumeNum;
    std::vector<GameObject*>     _volumeBarsFocus;
    std::vector<GameObject*>     _volumeBarsNonFocus;
    std::vector<GameObject*>     _volumeNumFocus;
    std::vector<GameObject*>     _volumeNumNonFocus;
    class PreferencesManager*   _preferencesManager;

    int _currentVolume  = MaxVolume;
    bool _isFocus       = false;
};
