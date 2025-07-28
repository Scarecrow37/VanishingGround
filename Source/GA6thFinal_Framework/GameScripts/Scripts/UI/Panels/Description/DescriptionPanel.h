#pragma once
#include "UI/Panels/Horizontal/HorizontalPanel.h"

enum class ElementType
{
    TEXT,
    IMAGE
};

struct TextAttributes
{
    std::string Content;
    Color Color;
};

struct ImageAttributes
{
    File::GuidRef Guid;
};

struct ElementData
{
    ElementType Type;
    std::variant<TextAttributes, ImageAttributes> Data;
};

class DescriptionPanel : public HorizontalPanel
{
    USING_PROPERTY(DescriptionPanel)

public:
    DescriptionPanel();

public:
    REFLECT_PROPERTY(FontPath, Description, CopyPath)

    GETTER_ONLY(std::string, FontPath) { return _guidRef.ToPath().string(); }
    PROPERTY(FontPath)

    GETTER(std::string, Description) { return ReflectFields->Description; }
    SETTER(std::string, Description)
    {
        ReflectFields->Description = value;
        UpdateContent();
    }
    PROPERTY(Description)

    GETTER(std::string, CopyPath) { return _copyGuidRef.string(); }
    SETTER(std::string, CopyPath) {}
    PROPERTY(CopyPath)

protected:
    void DeserializedReflectEvent() override;

private:
    void UpdateContent();

protected:
    REFLECT_FIELDS_BEGIN(HorizontalPanel)
    std::string Guid;
    std::string Description;
    REFLECT_FIELDS_END(DescriptionPanel)

private:
    File::GuidRef _guidRef;
    File::GuidRef _copyGuidRef;
};