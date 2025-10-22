#pragma once


class EnableButton : public UISFXNavigationComponent,public InputReceiver
{
    USING_PROPERTY(EnableButton)

public:
    EnableButton();
    ~EnableButton() override;

public:
    void Awake() override;
    void Reset() override;
    void Update() override;
    void OnEnable() override;

public:
    REFLECT_PROPERTY(OnFocus,OnNonFocus,OffFocus,OffNonFocus,CurrentOption)

    GETTER_ONLY(std::string, OnFocus) { return _onFocusImage.ToPath().string(); }
    PROPERTY(OnFocus)

    GETTER_ONLY(std::string, OnNonFocus) { return _onNonFocusImage.ToPath().string(); }
    PROPERTY(OnNonFocus)

    GETTER_ONLY(std::string, OffFocus) { return _offFocusImage.ToPath().string(); }
    PROPERTY(OffFocus)

    GETTER_ONLY(std::string, OffNonFocus) { return _offNonFocusImage.ToPath().string(); }
    PROPERTY(OffNonFocus)
    
    GETTER_ONLY(std::string, CurrentOption) { return _currentOption; }
    PROPERTY(CurrentOption)

protected:
    void FocusIn(FocusCallType callType) override;
    void FocusOut(FocusCallType callType) override;
    void Submit() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
private:
    void ChangeOptionDpad(const Input::Controller&);
    void ChangeOptionStick(const Input::Controller&);
    void FocusPref(bool isfocus);
    void GetChildObject();
    void ChangeOption();

protected:
    REFLECT_FIELDS_BEGIN(UISFXNavigationComponent)
    std::string OnFocusGuid;
    std::string OnNonFocusGuid;
    std::string OffFocusGuid;
    std::string OffNonFocusGuid;
    std::string CurrentOptionStr;
    REFLECT_FIELDS_END(EnableButton)
private:
    File::Guid _onFocusImage;
    File::Guid _onNonFocusImage;
    File::Guid _offFocusImage;
    File::Guid _offNonFocusImage;
    bool          _isOptionOn = true;
    bool          _isFocus    = false;
    bool          _isOptionDirty = false;

private:
    GameObject* _leftArrow;
    GameObject* _rightArrow;
    GameObject* _pref;
    class PreferencesManager* _preferencesManager;

    int _currentOptionInt = 0;
    std::string _currentOption;
};
