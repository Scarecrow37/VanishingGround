#pragma once
#include "UmFramework.h"

class TextureLMH : public UISFXNavigationComponent, public InputReceiver
{
    USING_PROPERTY(TextureLMH)
    enum TextureQuality
    {
        TEXTURE_QUALITY_LOW,
        TEXTURE_QUALITY_MEDIUM,
        TEXTURE_QUALITY_HIGH,
        TEXTURE_QUALITY_END
    };

public:
    TextureLMH();
    ~TextureLMH() override;

public:
    void Awake() override;
    void Start() override;
    void Reset() override;
    void Update() override;

public:
    REFLECT_PROPERTY()
protected:
    void FocusIn(FocusCallType callType) override;
    void FocusOut(FocusCallType callType) override;
    void Submit() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

private:
    void FocusPref(bool isfocus);
    void GetChildObject();
    void SetQuality(int quality);
    void UpQuality(const Input::Controller& controller);
    void DownQuality(const Input::Controller& controller);
    void UpDownStickQuality(const Input::Controller& controller);

protected:
    REFLECT_FIELDS_BEGIN(UISFXNavigationComponent)
    REFLECT_FIELDS_END(TextureLMH)
private:
    bool _isFocus        = false;
    bool _isOptionDirty  = false;
    bool _isOptionUp     = false;
    bool _isOptionDown   = false;
    int  _quality        = TEXTURE_QUALITY_HIGH;

private:
    GameObject* _leftArrow;
    GameObject* _rightArrow;
    GameObject* _pref;
    GameObject* _focus[TextureQuality::TEXTURE_QUALITY_END];
    GameObject* _nonFocus[TextureQuality::TEXTURE_QUALITY_END];

    class PreferencesManager* _preferencesManager;
};
