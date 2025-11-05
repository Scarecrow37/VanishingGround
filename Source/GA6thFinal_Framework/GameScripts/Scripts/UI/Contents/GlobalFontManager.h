#pragma once

class IFontAppearance;

class GlobalFontManager : public Component
{
    USING_PROPERTY(GlobalFontManager)

public:
    GlobalFontManager();

public:
    REFLECT_PROPERTY(FontWeight)

    GETTER(float, FontWeight) { return ReflectFields->FontWeight; }
    SETTER(float, FontWeight)
    {
        ReflectFields->FontWeight = std::clamp(value, 0.0f, 4.0f);
        UpdateFontWeight();
    }
    PROPERTY(FontWeight)

protected:
    void Awake() override;

    void ReFindFontAppearances();

private:
    void FindFontAppearances();
    void ClearFontAppearances();
    void UpdateFontWeight();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    float FontWeight = 0.5f;
    REFLECT_FIELDS_END(GlobalFontManager)

private:
    std::vector<std::weak_ptr<IFontAppearance>> _fontAppearances;
};