#pragma once

class FadeUIComponent;
class SpriteAnimationElement;
class ChildsAnimationsController : public Component
{
    USING_PROPERTY(ChildsAnimationsController)

public:
    ChildsAnimationsController();
    ~ChildsAnimationsController() override;

public:
    void FindAnimations();
    void FadeIn(size_t index);
    void FadeInWithTag(std::string_view tag);
    void FadeOut(size_t index);
    void FadeOutWithTag(std::string_view tag);

    void StartAnimation(size_t index);
    void StartAnimationWithTag(std::string_view tag);
    void StopAnimation(size_t index);
    void StopAnimationWithTag(std::string_view tag);

    bool IsFadeComplete(size_t index) const;
    void CompleteFadeImmediately(size_t index);

    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(ChildsAnimationsController)

    void Awake() override;

private:
    std::vector<FadeUIComponent*>        _fadeUIs;
    std::vector<SpriteAnimationElement*> _spriteAnimations;

    std::unordered_map<std::string_view, std::vector<FadeUIComponent*>>        _fadeUIsWithTag;
    std::unordered_map<std::string_view, std::vector<SpriteAnimationElement*>> _spriteAnimationsWithTag;

};

