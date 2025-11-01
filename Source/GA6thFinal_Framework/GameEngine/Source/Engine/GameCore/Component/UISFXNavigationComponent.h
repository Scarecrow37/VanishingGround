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
    void FocusIn(FocusCallType callType) override;
    void SetFocusInAudioID(const std::string& audioID);

protected:
    REFLECT_FIELDS_BEGIN(UINavigationComponent)
    REFLECT_FIELDS_END(UISFXNavigationComponent)

    inline static constexpr const char* DEFAULT_FOCUS_IN_SOUND_ID = "-901000";

private:
    std::string _focusInAudioID = "0";

};
