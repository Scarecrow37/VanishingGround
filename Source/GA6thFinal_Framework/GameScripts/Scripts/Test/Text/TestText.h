#pragma once
#include "UmFramework.h"

class FontRenderer;
class TestText : public Component
{
    USING_PROPERTY(TestText)
public:
    REFLECT_PROPERTY()

public:
    TestText();
    virtual ~TestText();

protected:
    void Reset() override;
    void ImGuiDrawPropertysEvent() override;
    void DeserializedReflectEvent() override;

private:
    void LoadFont();

protected:
    File::GuidRef _guidRef;

    REFLECT_FIELDS_BEGIN(Component)
    std::string Guid;
    std::string Text;
    REFLECT_FIELDS_END(TestText)

private:
    std::unique_ptr<FontRenderer> _fontRenderer;
};
