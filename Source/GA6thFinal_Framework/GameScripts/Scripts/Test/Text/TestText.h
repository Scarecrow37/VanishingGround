#pragma once
#include "UmFramework.h"

class FontRenderer;
class TestText : public Component
{
    USING_PROPERTY(TestText)
public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return _guidRef.ToPath().string(); }
    PROPERTY(FilePath)

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
    std::string          Guid;
    std::string          Text;
    std::array<float, 4> Color;
    std::array<float, 3> Position;
    std::array<float, 2> Scale;
    std::array<float, 2> Origin;
    float                Rotation;
    REFLECT_FIELDS_END(TestText)

private:
    std::unique_ptr<FontRenderer> _fontRenderer;
};
