#pragma once

class FadeImageElement;
class FadeTextElement;
class FadeDescriptionPanel;
class ExcelDataBase;

class IntroManager : public Component, public InputReceiver
{
    USING_PROPERTY(IntroManager)

    enum class Step : unsigned char
    {
        WAIT_INTRO_DESCRIPTION,
        FADE_IN_INTRO_DESCRIPTION,
        WAIT_LEVEL_SELECTION,
        FADE_IN_LEVEL_SELECTION,
        WAIT_PROMPT,
        FADE_IN_PROMPT,
        END
    };

    static constexpr std::string_view TAG_INTRO_DESCRIPTION      = "Intro Description";
    static constexpr std::string_view TAG_NORMAL_LEVEL_TEXT      = "Normal Level Text";
    static constexpr std::string_view TAG_HARD_LEVEL_TEXT        = "Hard Level Text";
    static constexpr std::string_view TAG_PROMPT_TEXT            = "Intro Prompt";
    static constexpr std::string_view TAG_NORMAL_LEVEL_SELECTION = "Normal Level Selection";
    static constexpr std::string_view TAG_HARD_LEVEL_SELECTION   = "Hard Level Selection";

    static constexpr std::u8string_view SHEET_NAME                            = u8"텍스트";
    static constexpr std::u8string_view COLUMN_KEY_ID                         = u8"ID";
    static constexpr std::u8string_view COLUMN_KEY_CONTENT                    = u8"Content";
    static constexpr std::u8string_view BOOK_SELECT_INTRO_DESC_ID             = u8"803000";
    static constexpr std::u8string_view BOOK_SELECT_INTRO_NORMAL_ID           = u8"803001";
    static constexpr std::u8string_view BOOK_SELECT_INTRO_HARD_ID             = u8"803002";
    static constexpr std::u8string_view BOOK_SELECT_INTRO_BOOK_NAME_PROMPT_ID = u8"803003";

public:
    IntroManager();

public:
    REFLECT_PROPERTY(FadeDuration, DescriptionDelay, LevelSelectDelay, PromptDelay, NextScene)

    GETTER(float, FadeDuration) { return ReflectFields->FadeDuration; }
    SETTER(float, FadeDuration) { ReflectFields->FadeDuration = std::max(0.1f, value); }
    PROPERTY(FadeDuration)

    GETTER(float, DescriptionDelay) { return ReflectFields->DescriptionDelay; }
    SETTER(float, DescriptionDelay) { ReflectFields->DescriptionDelay = std::max(0.0f, value); }
    PROPERTY(DescriptionDelay)

    GETTER(float, LevelSelectDelay) { return ReflectFields->LevelSelectDelay; }
    SETTER(float, LevelSelectDelay) { ReflectFields->LevelSelectDelay = std::max(0.0f, value); }
    PROPERTY(LevelSelectDelay)

    GETTER(float, PromptDelay) { return ReflectFields->PromptDelay; }
    SETTER(float, PromptDelay) { ReflectFields->PromptDelay = std::max(0.0f, value); }
    PROPERTY(PromptDelay)

    GETTER_ONLY(std::string, NextScene) { return File::Guid(ReflectFields->NextScene).ToPath().string(); }
    PROPERTY(NextScene)

protected:
    void Awake() override;
    void Update() override;

private:
    float GetWaitDescriptionTime() const;
    float GetFadeDescriptionTime() const;
    float GetWaitLevelSelectionTime() const;
    float GetFadeLevelSelectionTime() const;
    float GetWaitPromptTime() const;
    float GetFadePromptTime() const;

    void LoadNextScene() const;
    void FindComponents();
    std::unique_ptr<ExcelDataBase> SetupData();
    void                           SetupComponent(const std::unique_ptr<ExcelDataBase>& data);
    void                           ResetFade();

    void SkipStep(const Input::Controller& controller);
    void SelectNormal(const Input::Controller& controller);
    void SelectNormal();
    void SelectHard(const Input::Controller& controller);

    template <typename T>
    static T* GetElement(const std::string& tag);

protected:
    REFLECT_FIELDS_BEGIN(Component)
    float       FadeDuration     = 1.0f;
    float       DescriptionDelay = 2.0f;
    float       LevelSelectDelay = 2.0f;
    float       PromptDelay      = 1.0f;
    std::string NextScene;
    REFLECT_FIELDS_END(IntroManager)

private:
    Step  _step;
    float _elapsedTime = 0.0f;
    bool  _isLevelSelected;
    bool  _isSelectHard;

    std::weak_ptr<FadeDescriptionPanel> _introDescription;
    std::weak_ptr<FadeTextElement>      _normalLevelText;
    std::weak_ptr<FadeTextElement>      _hardLevelText;
    std::weak_ptr<FadeTextElement>      _promptText;
    std::weak_ptr<FadeImageElement>     _normalSelection;
    std::weak_ptr<FadeImageElement>     _hardSelection;
};

template <typename T>
T* IntroManager::GetElement(const std::string& tag)
{
    const std::weak_ptr<GameObject> introDescriptionObjectWeak = GameObject::FindWithTag(tag);

    if (const std::shared_ptr introDescriptionObject = introDescriptionObjectWeak.lock();
        nullptr != introDescriptionObject)
    {
        T* component = introDescriptionObject->GetComponent<T>();
        return component;
    }
    return nullptr;
}