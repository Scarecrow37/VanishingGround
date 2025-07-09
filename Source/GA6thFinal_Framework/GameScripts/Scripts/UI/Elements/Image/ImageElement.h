#pragma once
#include "../../Base/UIComponent.h"

class ImageElement : public UIComponent
{
    USING_PROPERTY(ImageElement)

public:
    ImageElement();
    ~ImageElement() override;

    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return _guidRef.ToPath().string(); }
    PROPERTY(FilePath)

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    std::string Guid;
    REFLECT_FIELDS_END(ImageElement)

    void Reset() override;

private:
    std::unique_ptr<SpriteRenderer> _renderer;
    File::GuidRef               _guidRef;
};