#pragma once
#include <Utility/SingletonHelper.h>

class OverlayPanel;
class ImageElement;

class CombatUIManager : public Component, public InputReceiver
{
    USING_PROPERTY(CombatUIManager)

public:
    CombatUIManager();
    ~CombatUIManager() override;

public:
    /// <summary>
    /// UI데이터를 갱신합니다.
    /// </summary>
    void Refresh();

protected:
    void Reset() override;
    void Awake() override;
    void ImGuiDrawPropertysEvent() override;
    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

private:
    SingletonObject<CombatUIManager> _singletonObject{this};
    SingletonComponent<CombatUIManager> _singletonComponent{this};

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(CombatUIManager)

private:
    void PreferencesKeyDown(const Input::Controller&);

};

