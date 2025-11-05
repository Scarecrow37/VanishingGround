#pragma once
#include "UI/Base/IFontAppearance/IFontAppearance.h"
#include "UI/Base/IOpacity/IOpacity.h"
#include "UI/Panels/Horizontal/HorizontalPanel.h"

enum class ElementType : unsigned char
{
    TEXT,
    IMAGE,
    BREAK,
};

struct TextAttributes
{
    std::string Content;
    Color Color;
};

struct ImageAttributes
{
    File::Guid Guid;
};

struct ElementData
{
    ElementType Type;
    std::variant<TextAttributes, ImageAttributes> Data;
};

class DescriptionPanel : public HorizontalPanel, public IOpacity, public IFontAppearance
{
    USING_PROPERTY(DescriptionPanel)

public:
    DescriptionPanel();

public:
    REFLECT_PROPERTY(FontPath, Description, FontScale, Alpha)

    GETTER_ONLY(std::string, FontPath) { return _Guid.ToPath().string(); }
    PROPERTY(FontPath)

    GETTER(std::string, Description) { return ReflectFields->Description; }
    SETTER(std::string, Description)
    {
        if (ReflectFields->Description != value)
        {
            ReflectFields->Description = value;
            UpdateContent();
        }
    }
    PROPERTY(Description)

    GETTER(float, FontScale) { return ReflectFields->FontScale; }
    SETTER(float, FontScale)
    {
        if (ReflectFields->FontScale != value)
        {
            ReflectFields->FontScale = value;
            UpdateContent();
        }
    }
    PROPERTY(FontScale)

    GETTER(float, Alpha) { return ReflectFields->Alpha; }
    SETTER(float, Alpha) { SetOpacity(value); }
    PROPERTY(Alpha)

public:
    void SetOpacity(float opacity) override;
    void SetFontWeight(float fontWeight) override;

protected:
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

    void Awake() override;

private:
    void UpdateContent();
    void EraseChild() const;
    void MakeChild();
    void UpdateAlpha();

protected:
    REFLECT_FIELDS_BEGIN(HorizontalPanel)
    std::string Guid;
    std::string Description;
    float       FontScale = 16.0f;
    float       Alpha     = 1.0f;
    REFLECT_FIELDS_END(DescriptionPanel)

private:
    File::Guid _Guid;

    float _fontWeight;
};