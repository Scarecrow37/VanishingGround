#pragma once
#include "../../Base/UIComponent.h"

class TextElement : public UIComponent
{
    USING_PROPERTY(TextElement)
public:
    REFLECT_PROPERTY(Content)
    GETTER(std::string, Content) { return ReflectFields->Content; }
    SETTER(std::string, Content) { ReflectFields->Content = value; }
    PROPERTY(Content)

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    std::string Content;
    REFLECT_FIELDS_END(TextElement)
};