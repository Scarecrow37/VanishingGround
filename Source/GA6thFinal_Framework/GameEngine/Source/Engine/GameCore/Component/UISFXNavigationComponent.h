#pragma once

class UISFXNavigationComponent : public UINavigationComponent
{
    USING_PROPERTY(UISFXNavigationComponent)

public:
    UISFXNavigationComponent();
    ~UISFXNavigationComponent() override;

public:
    REFLECT_PROPERTY(FocusInAudioID)

    GETTER(std::string, FocusInAudioID) { return _focusInAudioID; }
    SETTER(std::string, FocusInAudioID) { _focusInAudioID = value; }
    PROPERTY(FocusInAudioID)

protected:
    virtual void FocusIn(FocusCallType callType) override;
    virtual void Submit() override;

    void SetFocusInAudioID(const std::string& audioID);
    void SetSubmitAudioID(const std::string& audioID);

protected:
    REFLECT_FIELDS_BEGIN(UINavigationComponent)
    REFLECT_FIELDS_END(UISFXNavigationComponent)

    inline static constexpr const char* DEFAULT_FOCUS_IN_SOUND_ID = "-901000";
    inline static constexpr const char* DEFAULT_SUBMIT_SOUND_ID = "-901002";

private:
    std::string _focusInAudioID = "0";
    std::string _submitAudioID = "0";

};
