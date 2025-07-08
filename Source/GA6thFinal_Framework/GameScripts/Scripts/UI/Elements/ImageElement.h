#pragma once
#include "../Base/UIComponent.h"
#include "UmFramework.h"

class ImageElement : public UIComponent
{
    USING_PROPERTY(ImageElement)

public:
    ImageElement();

    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return _guidRef.ToPath().string(); }
    PROPERTY(FilePath)

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    std::string Guid;
    REFLECT_FIELDS_END(ImageElement)

private:
    File::GuidRef _guidRef;
};