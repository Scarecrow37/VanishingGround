#pragma once

class PreferencesButton : public Component
{
    USING_PROPERTY(PreferencesButton)

public:
    PreferencesButton();

public:
    void Awake() override;
    void Start() override;
    void Reset() override;
    void Update() override;

    void OnFocus(bool focus);

public:
    REFLECT_PROPERTY(OnFilePath,OffFilePath)

    GETTER_ONLY(std::string, OnFilePath) { return _onImage.ToPath().string(); }
    PROPERTY(OnFilePath)

    GETTER_ONLY(std::string, OffFilePath) { return _offImage.ToPath().string(); }
    PROPERTY(OffFilePath)
protected:
    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string OnGuid;
    std::string OffGuid;
    REFLECT_FIELDS_END(PreferencesButton)
private:
    File::Guid _onImage;
    File::Guid _offImage;
};