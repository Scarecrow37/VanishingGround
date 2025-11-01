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
    void  FindAnimations();
    void  Begin(size_t index);
    void  BeginWithTag(const std::string& tag);
    float FadeIn(size_t index);
    float FadeInWithTag(const std::string& tag);
    void  End(size_t index);
    void  EndWithTag(const std::string& tag);
    float FadeOut(size_t index);
    float FadeOutWithTag(const std::string& tag);

    float GetDurationWithTag(const std::string& tag);

    float StartAnimation(size_t index);
    float StartAnimationWithTag(const std::string& tag);
    void  StopAnimation(size_t index);
    void  StopAnimationWithTag(const std::string& tag);

    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(ChildsAnimationsController)

    void Added() override;

private:
    std::vector<FadeUIComponent*>        _fadeUIs;
    std::vector<SpriteAnimationElement*> _spriteAnimations;

    std::unordered_map<std::string, std::vector<FadeUIComponent*>>        _fadeUIsWithTag;
    std::unordered_map<std::string, std::vector<SpriteAnimationElement*>> _spriteAnimationsWithTag;

};

