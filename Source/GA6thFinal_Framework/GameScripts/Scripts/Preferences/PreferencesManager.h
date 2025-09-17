#pragma once 
#include "DLLExportDefine.h"

class PreferencesManager : public Component, public InputReceiver
{
    USING_PROPERTY(PreferencesManager)

public:
    PreferencesManager();
    ~PreferencesManager() override;

public:
    void Reset() override;
    void Awake() override;
    void Update() override;

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
    MVVM::Model<bool> _buttonEnable;
};
