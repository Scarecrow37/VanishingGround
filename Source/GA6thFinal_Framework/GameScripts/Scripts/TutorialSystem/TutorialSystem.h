#pragma once
#include "Utility/SingletonHelper.h"

class HoldingProgressImageElement;
class DescriptionPanel;
class TextElement;
class ImageElement;

struct Tutorial
{
    bool        IsCompleted;
    std::string Title;
    std::string Description;
    File::Guid  Image;
};

class TutorialSystem : public Component, public InputReceiver
{
    USING_PROPERTY(TutorialSystem)

    static constexpr std::string_view OBJECT_TAG_PANEL       = "Tutorial Panel";
    static constexpr std::string_view OBJECT_TAG_TITLE       = "Tutorial Title";
    static constexpr std::string_view OBJECT_TAG_DESCRIPTION = "Tutorial Description";
    static constexpr std::string_view OBJECT_TAG_IMAGE       = "Tutorial Image";
    static constexpr std::string_view OBJECT_TAG_CONFIRM     = "Tutorial Confirm";

    static constexpr std::u8string_view SHEET_NAME             = u8"튜토리얼 텍스트";
    static constexpr std::u8string_view COLUMN_KEY_ID          = u8"ID";
    static constexpr std::u8string_view COLUMN_KEY_TITLE       = u8"Title";
    static constexpr std::u8string_view COLUMN_KEY_DESCRIPTION = u8"Text Description";
    static constexpr std::u8string_view COLUMN_KEY_IMAGE       = u8"Image";


public:
    TutorialSystem();

public:
    REFLECT_PROPERTY(IsShown)

    GETTER_ONLY(bool, IsShown)
    {
        if (const auto panelComponent = _panel.lock())
            return panelComponent->ActiveInHierarchy;
        return false;
    }
    PROPERTY(IsShown)

public:
    void Show(int id);
    void Show(std::initializer_list<int> ids);
    void Hide();

protected:
    void Awake() override;
    void Start() override;
    void ImGuiDrawPropertysEvent() override;

private:
    void FindComponents();
    void SetupData();
    void SetupCallback();
    void HoldA(const Input::Controller& controller);
    void ReleaseA(const Input::Controller& controller);
    void ShowNextTutorialOrHide();

    void Lock();
    void Unlock();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TutorialSystem)

private:
    SingletonObject<TutorialSystem>    _singletonObject{this};
    SingletonComponent<TutorialSystem> _singletonComponent{this};

    std::unordered_map<int, Tutorial> _tutorials;
    std::deque<int>                   _pendingTutorials;

    std::weak_ptr<GameObject>                  _panel;
    std::weak_ptr<TextElement>                 _title;
    std::weak_ptr<DescriptionPanel>            _description;
    std::weak_ptr<ImageElement>                _image;
    std::weak_ptr<HoldingProgressImageElement> _confirm;
};