#pragma once

class SoundButton : public UISFXNavigationComponent, public InputReceiver
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
    REFLECT_PROPERTY(CurrentOption)

    GETTER_ONLY(std::string, CurrentOption) { return _currentOption; }
    PROPERTY(CurrentOption)

protected:
    void FocusIn(FocusCallType callType) override;
    void FocusOut(FocusCallType callType) override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

private:
    void ControlVolumeUp(const Input::Controller& controller);
    void ControlVolumeDown(const Input::Controller& controller);
    void ControlVolumeStick(const Input::Controller& controller);
    void GetChildObject();
    void ChangeVolume(int delta);
    void UpdateUIForFocus();

protected:
    REFLECT_FIELDS_BEGIN(UISFXNavigationComponent)
    std::string CurrentOptionStr;
    REFLECT_FIELDS_END(SoundButton)
private:
    GameObject*               _leftArrow;
    GameObject*               _rightArrow;
    GameObject*               _title[2];
    GameObject*               _volumeBar;
    GameObject*               _volumeNum;
    std::vector<GameObject*>  _volumeBarsFocus;
    std::vector<GameObject*>  _volumeBarsNonFocus;
    std::vector<GameObject*>  _volumeNumFocus;
    std::vector<GameObject*>  _volumeNumNonFocus;
    class PreferencesManager* _preferencesManager;

    int         _currentVolume = 10;
    bool        _isFocus       = false;
    bool        _isVolumeUp    = false;
    bool        _isVolumeDown  = false;
    bool        _isOptionDirty = false;
    int         _currentOptionInt = 0;
    std::string _currentOption;
};
