#pragma once

class TextElement;
class ImageElement;
class DescriptionPanel;

class WeaponView : public Component
{
    USING_PROPERTY(WeaponView)

public:
    WeaponView();
    ~WeaponView() override;

protected:
    void Awake() override;
    void Start() override;

private:
    void                 FindElements();
    static ImageElement* FindImageElement(const std::string& tag, GameObject& object);
    static TextElement*  FindTextElement(const std::string& tag, GameObject& object);
    static DescriptionPanel* FindDescriptionPanel(const std::string& tag, GameObject& object);

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(WeaponView)

private:
    ImageElement* _background;
    TextElement*  _weaponName;
    ImageElement* _weaponImage;
    TextElement*  _hitDamage;
    TextElement*  _criticalDamage;
    TextElement*  _speed;
    TextElement*  _attackCount;
    DescriptionPanel* _description1;
    DescriptionPanel* _description2;
};
