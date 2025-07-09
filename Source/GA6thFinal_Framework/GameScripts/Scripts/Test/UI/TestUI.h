#pragma once
#include "UmFramework.h"

class SpriteRenderer;
class TestUI : public Component
{
    USING_PROPERTY(TestUI)
public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return _guidRef.ToPath().string(); }
    PROPERTY(FilePath)

public:
    TestUI();
    virtual ~TestUI();

protected:
    void Reset() override;
    void ImGuiDrawPropertysEvent() override;
    void DeserializedReflectEvent() override;

private:
    void LoadTexture();

protected:
    File::GuidRef _guidRef;

    REFLECT_FIELDS_BEGIN(Component)
    std::string Guid;
    int         Type;
    REFLECT_FIELDS_END(TestUI)

private:
    std::unique_ptr<SpriteRenderer> _spriteRenderer;
};
