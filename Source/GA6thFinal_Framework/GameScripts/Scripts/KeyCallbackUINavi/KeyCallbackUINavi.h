#pragma once
class KeyCallbackUINavi : public UISFXNavigationComponent
{
    USING_PROPERTY(KeyCallbackUINavi)

public:
    KeyCallbackUINavi();
    ~KeyCallbackUINavi() override;

    inline static std::unordered_map<std::string, UmDelegate<>> EventListenerMapFocusIn;
    inline static std::unordered_map<std::string, UmDelegate<>> EventListenerMapFocusOut;
    inline static std::unordered_map<std::string, UmDelegate<>> EventListenerMapSubmit;

    inline static std::unordered_map<std::string, UmDelegate<>> EventListenerMapShowTooltips;
    inline static std::unordered_map<std::string, UmDelegate<>> EventListenerMapHideTooltips;

public:
    GETTER(const std::string&, CallbackKey) { return ReflectFields->CallbackKey; }
    SETTER(const std::string&, CallbackKey) { ReflectFields->CallbackKey = value; }
    // 콜백 함수 키
    PROPERTY(CallbackKey)

    REFLECT_PROPERTY(CallbackKey)
protected:
    REFLECT_FIELDS_BEGIN(UISFXNavigationComponent)
    std::string CallbackKey;
    REFLECT_FIELDS_END(KeyCallbackUINavi)

    void Added() override;

    void FocusIn(FocusCallType callType) override;
    void FocusOut(FocusCallType callType) override;
    void Submit() override;

    void ShowTooltips() override;
    void HideTooltips() override;
};

