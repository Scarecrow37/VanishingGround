#pragma once

class PreferencesManager : public Component, public InputReceiver
{
    USING_PROPERTY(PreferencesManager)
public:
    PreferencesManager();
    ~PreferencesManager() override;

public:
    void Reset() override;
    void Awake() override;

public:
    void SetGraphicsOptions(std::string_view option, bool enable);

private:
    void OnPreferencesWindow(const Input::Controller&);
    // 임시
    void OffPreferencesWindow(const Input::Controller&);

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(PreferencesManager)

private:
    GameObject*                        _preferencesPannel;
    std::map<std::string, GameObject*> _graphicsOption;
};
