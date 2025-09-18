#pragma once


class EnableButton : public UINavigationComponent,public InputReceiver
{
    USING_PROPERTY(EnableButton)

public:
    EnableButton();
    ~EnableButton() override;

public:
    void Awake() override;
    void Start() override;
    void Reset() override;
    void Update() override;

public:
    REFLECT_PROPERTY(OnFocus,OnNonFocus,OffFocus,OffNonFocus)

    GETTER_ONLY(std::string, OnFocus) { return _onFocusImage.ToPath().string(); }
    PROPERTY(OnFocus)

    GETTER_ONLY(std::string, OnNonFocus) { return _onNonFocusImage.ToPath().string(); }
    PROPERTY(OnNonFocus)

    GETTER_ONLY(std::string, OffFocus) { return _offFocusImage.ToPath().string(); }
    PROPERTY(OffFocus)

    GETTER_ONLY(std::string, OffNonFocus) { return _offNonFocusImage.ToPath().string(); }
    PROPERTY(OffNonFocus)

protected:
    void FocusIn() override;
    void FocusOut() override;
    void Submit() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
private:
    void ChangeOptionDpad(const Input::Controller&);
    void ChangeOptionStick(const Input::Controller&);
    void FocusPref(bool isfocus);
    void GetChildObject();

protected:
    REFLECT_FIELDS_BEGIN(UINavigationComponent)
    std::string OnFocusGuid;
    std::string OnNonFocusGuid;
    std::string OffFocusGuid;
    std::string OffNonFocusGuid;
    REFLECT_FIELDS_END(EnableButton)
private:
    File::GuidRef _onFocusImage;
    File::GuidRef _onNonFocusImage;
    File::GuidRef _offFocusImage;
    File::GuidRef _offNonFocusImage;
    bool          _isOptionOn = true;
    bool          _isFocus    = false;

private:
    GameObject* _leftArrow;
    GameObject* _rightArrow;
    GameObject* _pref;
};
