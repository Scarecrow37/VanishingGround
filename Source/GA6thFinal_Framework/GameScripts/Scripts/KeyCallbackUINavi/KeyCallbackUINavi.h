#pragma once
class KeyCallbackUINavi : public UISFXNavigationComponent
{
    USING_PROPERTY(KeyCallbackUINavi)

public:
    KeyCallbackUINavi();
    ~KeyCallbackUINavi() override;

    using EventListenerMap = std::unordered_map<std::string, UmDelegate<>>;
    inline static EventListenerMap EventListenerMapFocusIn;
    inline static EventListenerMap EventListenerMapFocusOut;
    inline static EventListenerMap EventListenerMapSubmit;

    inline static EventListenerMap EventListenerMapShowTooltips;
    inline static EventListenerMap EventListenerMapHideTooltips;

    static std::pair<UmDelegate<>*, UmDelegate<>::Handle> AddCallback(UmDelegate<>& listener, const std::function<void()>& callback);

    static std::pair<UmDelegate<>*, UmDelegate<>::Handle> AddCallbackFocusIn(const std::string& key, const std::function<void()>& callback);
    static std::pair<UmDelegate<>*, UmDelegate<>::Handle> AddCallbackFocusOut(const std::string& key, const std::function<void()>& callback);
    static std::pair<UmDelegate<>*, UmDelegate<>::Handle> AddCallbackSubmit(const std::string& key, const std::function<void()>& callback);
    static std::pair<UmDelegate<>*, UmDelegate<>::Handle> AddCallbackShowTooltips(const std::string& key, const std::function<void()>& callback);
    static std::pair<UmDelegate<>*, UmDelegate<>::Handle> AddCallbackHideTooltips(const std::string& key, const std::function<void()>& callback);

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

